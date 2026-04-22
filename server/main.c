#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "server.h"
#include "protocol.h"
#include "config.h"
#include "auth.h"
#include "parser.h"
#include "client_manager.h"
#include "session.h"
#include "game_logic.h"

static void run_session(Client clientes[])
{
    Session sesion;
    GameState game;
    char buffer[BUFFER_SIZE];
    int i;
    int sesion_activa = 1;

    init_session(&sesion);
    init_game(&game);

    set_session_state(&sesion, WAITING_AUTH);

    printf("\n[HIJO] Los dos clientes ya están conectados\n");

    while (sesion_activa)
    {
        for (i = 0; i < MAX_CLIENTES; i++)
        {
            int leidos;
            int resultado_auth;
            ParsedMessage msg;

            if (!clientes[i].conectado)
            {
                continue;
            }

            if (sesion.estado == ACTIVE)
            {
                int jugador_esperado;

                if (secret_word_ready(&game))
                {
                    jugador_esperado = game.rm.guesser_id;
                }
                else
                {
                    jugador_esperado = game.rm.setter_id;
                }

                if (i != jugador_esperado)
                {
                    continue;
                }
            }

            leidos = receive_message(clientes[i].socket_fd, buffer, BUFFER_SIZE);

            if (leidos == 0)
            {
                int otro = (i == 0) ? 1 : 0;

                printf("[HIJO] Cliente %d se desconecto\n", i + 1);
                close(clientes[i].socket_fd);
                reset_client(&clientes[i]);
                set_session_state(&sesion, INTERRUPTED);

                if (clientes[otro].conectado)
                {
                    send_message(clientes[otro].socket_fd, RESP_OPPONENT_DISCONNECTED);
                }

                sesion_activa = 0;
                break;
            }

            if (leidos < 0)
            {
                printf("[HIJO] Error al recibir mensaje del cliente %d\n", i + 1);
                set_session_state(&sesion, INTERRUPTED);
                sesion_activa = 0;
                break;
            }

            printf("[HIJO] Mensaje del cliente %d: %s\n", i + 1, buffer);

            parse_message(buffer, &msg);

            if (msg.partes == 3 && strcmp(msg.comando, CMD_LOGIN) == 0)
            {
                resultado_auth = validate_credentials(RUTA_USERS, msg.arg1, msg.arg2);

                if (resultado_auth == 1)
                {
                    set_client_authenticated(&clientes[i], msg.arg1);
                    send_message(clientes[i].socket_fd, RESP_LOGIN_OK);
                    printf("[HIJO] Cliente %d autenticado como %s\n", i + 1, clientes[i].username);

                    if (are_both_authenticated(clientes, MAX_CLIENTES))
                    {
                        set_session_state(&sesion, READY);
                        send_message(clientes[0].socket_fd, RESP_START_GAME);
                        send_message(clientes[1].socket_fd, RESP_START_GAME);

                        start_round(&game.rm, 0, 1);
                        set_session_state(&sesion, ACTIVE);

                        send_message(clientes[0].socket_fd, RESP_YOUR_TURN_SETWORD);
                        send_message(clientes[1].socket_fd, RESP_WAITING_PLAYER);

                        printf("[HIJO] Ambos clientes autenticados. Sesion lista para iniciar\n");
                    }
                    else
                    {
                        send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                    }
                }
                else if (resultado_auth == 0)
                {
                    send_message(clientes[i].socket_fd, RESP_LOGIN_FAIL);
                    printf("[HIJO] Cliente %d fallo autenticacion\n", i + 1);
                }
                else
                {
                    send_message(clientes[i].socket_fd, RESP_LOGIN_FAIL);
                    printf("[HIJO] Error al abrir archivo de credenciales\n");
                }
            }
            else if (msg.partes == 3 && strcmp(msg.comando, CMD_REGISTER) == 0)
            {
                int existe;
                int registrado;

                existe = user_exists(RUTA_USERS, msg.arg1);

                if (existe == 1)
                {
                    send_message(clientes[i].socket_fd, RESP_ERROR_USER_EXISTS);
                    printf("[HIJO] Cliente %d intento registrar un usuario existente: %s\n", i + 1, msg.arg1);
                }
                else if (existe == 0)
                {
                    registrado = register_user(RUTA_USERS, msg.arg1, msg.arg2);

                    if (registrado == 1)
                    {
                        send_message(clientes[i].socket_fd, RESP_REGISTER_OK);
                        printf("[HIJO] Cliente %d registro al usuario %s correctamente\n", i + 1, msg.arg1);
                    }
                    else
                    {
                        send_message(clientes[i].socket_fd, RESP_REGISTER_FAIL);
                        printf("[HIJO] Error al registrar usuario %s\n", msg.arg1);
                    }
                }
                else
                {
                    send_message(clientes[i].socket_fd, RESP_REGISTER_FAIL);
                    printf("[HIJO] Error al abrir archivo de credenciales\n");
                }
            }
            else if (msg.partes == 2 && strcmp(msg.comando, CMD_SETWORD) == 0)
            {
                if (!clientes[i].autenticado)
                {
                    send_message(clientes[i].socket_fd, RESP_ERROR_AUTH_REQUIRED);
                }
                else if (!is_round_active(&game.rm))
                {
                    send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                }
                else if (i != game.rm.setter_id)
                {
                    send_message(clientes[i].socket_fd, RESP_ERROR_NOT_YOUR_TURN);
                }
                else if (secret_word_ready(&game))
                {
                    send_message(clientes[i].socket_fd, RESP_ERROR_NOT_YOUR_TURN);
                }
                else
                {
                    int otro = game.rm.guesser_id;

                    if (set_secret_word(&game, msg.arg1))
                    {
                        send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                        send_message(clientes[otro].socket_fd, RESP_YOUR_TURN_GUESS);
                        printf("[HIJO] Jugador %d establecio la palabra secreta\n", i + 1);
                    }
                    else
                    {
                        send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_WORD);
                        send_message(clientes[i].socket_fd, RESP_YOUR_TURN_SETWORD);
                        printf("[HIJO] Jugador %d envio una palabra invalida\n", i + 1);
                    }
                }
            }
            else if (msg.partes == 2 && strcmp(msg.comando, CMD_GUESS) == 0)
            {
                if (!clientes[i].autenticado)
                {
                    send_message(clientes[i].socket_fd, RESP_ERROR_AUTH_REQUIRED);
                }
                else if (!is_round_active(&game.rm))
                {
                    send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                }
                else if (i != game.rm.guesser_id)
                {
                    send_message(clientes[i].socket_fd, RESP_ERROR_NOT_YOUR_TURN);
                }
                else if (!secret_word_ready(&game))
                {
                    send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                }
                else
                {
                    int result[5];
                    char result_msg[BUFFER_SIZE];
                    int guessed_correctly;

                    if (process_guess(&game, msg.arg1, result))
                    {
                        snprintf(
                            result_msg,
                            BUFFER_SIZE,
                            "%s %d %d %d %d %d",
                            RESP_RESULT_PREFIX,
                            result[0], result[1], result[2], result[3], result[4]);

                        send_message(clientes[i].socket_fd, result_msg);
                        printf("[HIJO] Resultado enviado al jugador %d: %s\n", i + 1, result_msg);

                        if (current_round_over(&game, result))
                        {
                            guessed_correctly = current_round_won(result);

                            register_round_result(&game, game.rm.guesser_id, guessed_correctly);
                            end_round(&game.rm);

                            send_message(clientes[0].socket_fd, RESP_ROUND_END);
                            send_message(clientes[1].socket_fd, RESP_ROUND_END);

                            if (game_over(&game))
                            {
                                GameResult final;

                                final = final_result(&game);

                                if (final == BOTH_CORRECT)
                                {
                                    send_message(clientes[0].socket_fd, RESP_GAME_OVER_BOTH);
                                    send_message(clientes[1].socket_fd, RESP_GAME_OVER_BOTH);
                                }
                                else if (final == ONLY_P1)
                                {
                                    send_message(clientes[0].socket_fd, RESP_GAME_OVER_J1);
                                    send_message(clientes[1].socket_fd, RESP_GAME_OVER_J1);
                                }
                                else if (final == ONLY_P2)
                                {
                                    send_message(clientes[0].socket_fd, RESP_GAME_OVER_J2);
                                    send_message(clientes[1].socket_fd, RESP_GAME_OVER_J2);
                                }
                                else
                                {
                                    send_message(clientes[0].socket_fd, RESP_GAME_OVER_NONE);
                                    send_message(clientes[1].socket_fd, RESP_GAME_OVER_NONE);
                                }

                                end_game(&game);
                                set_session_state(&sesion, ENDED);
                                sesion_activa = 0;
                                break;
                            }
                            else
                            {
                                prepare_next_round(&game);

                                send_message(clientes[game.rm.setter_id].socket_fd, RESP_YOUR_TURN_SETWORD);
                                send_message(clientes[game.rm.guesser_id].socket_fd, RESP_WAITING_PLAYER);

                                printf("[HIJO] Preparada la ronda %d\n", game.rm.current_round);
                            }
                        }
                        else
                        {
                            send_message(clientes[i].socket_fd, RESP_YOUR_TURN_GUESS);
                        }
                    }
                    else
                    {
                        send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_WORD);
                        send_message(clientes[i].socket_fd, RESP_YOUR_TURN_GUESS);
                        printf("[HIJO] Jugador %d envio un intento invalido\n", i + 1);
                    }
                }
            }
            else if (msg.partes == 1 && strcmp(msg.comando, CMD_DISCONNECT) == 0)
            {
                int otro = (i == 0) ? 1 : 0;

                printf("[HIJO] Cliente %d solicito desconexion\n", i + 1);
                close(clientes[i].socket_fd);
                reset_client(&clientes[i]);
                set_session_state(&sesion, INTERRUPTED);

                if (clientes[otro].conectado)
                {
                    send_message(clientes[otro].socket_fd, RESP_OPPONENT_DISCONNECTED);
                }

                sesion_activa = 0;
                break;
            }
            else if (msg.partes >= 1 && strcmp(msg.comando, CMD_LOGIN) == 0)
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_FORMAT);
                printf("[HIJO] Cliente %d envio LOGIN con formato invalido\n", i + 1);
            }
            else if (msg.partes >= 1 && strcmp(msg.comando, CMD_REGISTER) == 0)
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_FORMAT);
                printf("[HIJO] Cliente %d envio REGISTER con formato invalido\n", i + 1);
            }
            else if (msg.partes >= 1 && strcmp(msg.comando, CMD_SETWORD) == 0)
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_FORMAT);
                printf("[HIJO] Cliente %d envio SETWORD con formato invalido\n", i + 1);
            }
            else if (msg.partes >= 1 && strcmp(msg.comando, CMD_GUESS) == 0)
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_FORMAT);
                printf("[HIJO] Cliente %d envio GUESS con formato invalido\n", i + 1);
            }
            else
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_COMMAND);
                printf("[HIJO] Cliente %d envio comando invalido\n", i + 1);
            }
        }
    }

    for (i = 0; i < MAX_CLIENTES; i++)
    {
        if (clientes[i].conectado)
        {
            close(clientes[i].socket_fd);
            reset_client(&clientes[i]);
        }
    }

    set_session_state(&sesion, ENDED);
    printf("[HIJO] Sesion finalizada\n");
}

int main()
{
    int servidor_fd;
    Client clientes[MAX_CLIENTES];
    int i;

    /* Evita zombies de hijos terminados */
    signal(SIGCHLD, SIG_IGN);

    servidor_fd = create_server();
    if (servidor_fd < 0)
    {
        return 1;
    }

    printf("[PADRE] Servidor principal listo en puerto %d\n", PUERTO);

    while (1)
    {
        /* Reiniciar arreglo de clientes para una nueva sesión */
        for (i = 0; i < MAX_CLIENTES; i++)
        {
            init_client(&clientes[i]);
        }

        /* Emparejar 2 clientes */
        for (i = 0; i < MAX_CLIENTES; i++)
        {
            int socket_cliente;

            printf("[PADRE] Esperando al cliente %d de la nueva sesión...\n", i + 1);

            socket_cliente = accept_client(servidor_fd);
            if (socket_cliente < 0)
            {
                perror("[PADRE] Error aceptando cliente");
                close(servidor_fd);
                return 1;
            }

            set_client_connected(&clientes[i], socket_cliente);
            printf("[PADRE] Cliente %d asignado a la sesión actual\n", i + 1);
        }

        /* Crear proceso hijo para atender esta sesión */
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("[PADRE] Error en fork");

            for (i = 0; i < MAX_CLIENTES; i++)
            {
                if (clientes[i].conectado)
                {
                    close(clientes[i].socket_fd);
                    reset_client(&clientes[i]);
                }
            }

            continue;
        }

        if (pid == 0)
        {
            /* HIJO: atiende la sesión */
            close(servidor_fd);

            printf("[HIJO] Iniciando sesión de juego\n");
            run_session(clientes);

            for (i = 0; i < MAX_CLIENTES; i++)
            {
                if (clientes[i].conectado)
                {
                    close(clientes[i].socket_fd);
                    reset_client(&clientes[i]);
                }
            }

            printf("[HIJO] Proceso de sesión terminado\n");
            exit(0);
        }
        else
        {
            /* PADRE: cierra sus copias de los sockets y vuelve a esperar otra sesión */
            printf("[PADRE] Sesión delegada al hijo PID=%d\n", pid);

            for (i = 0; i < MAX_CLIENTES; i++)
            {
                if (clientes[i].conectado)
                {
                    close(clientes[i].socket_fd);
                    reset_client(&clientes[i]);
                }
            }
        }
    }

    close(servidor_fd);
    return 0;
}