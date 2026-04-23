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
    /* Estado general de la sesión y del juego */
    Session sesion;
    GameState game;
    char buffer[BUFFER_SIZE];
    int i;
    int sesion_activa = 1;

    /* Inicializa sesión y lógica del juego */
    init_session(&sesion);
    init_game(&game);

    set_session_state(&sesion, WAITING_AUTH);

    printf("\n[HIJO] Los dos clientes ya están conectados\n");

    /* Ciclo principal de la partida entre los 2 clientes */
    while (sesion_activa)
    {
        for (i = 0; i < MAX_CLIENTES; i++)
        {
            int leidos;
            int resultado_auth;
            ParsedMessage msg;

            /* Salta clientes no conectados */
            if (!clientes[i].conectado)
            {
                continue;
            }

            /* Durante el juego, solo lee al jugador cuyo turno corresponde */
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

            /* Recibe mensaje del cliente actual */
            leidos = receive_message(clientes[i].socket_fd, buffer, BUFFER_SIZE);

            /* Si un cliente se desconecta, se interrumpe la sesión */
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

            /* Si ocurre error al recibir, termina la sesión */
            if (leidos < 0)
            {
                printf("[HIJO] Error al recibir mensaje del cliente %d\n", i + 1);
                set_session_state(&sesion, INTERRUPTED);
                sesion_activa = 0;
                break;
            }

            printf("[HIJO] Mensaje del cliente %d: %s\n", i + 1, buffer);

            /* Convierte el texto recibido en comando + argumentos */
            parse_message(buffer, &msg);

            /* LOGIN: valida credenciales y, si ambos entran, arranca la partida */
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

                        /* Inicia ronda 1: jugador 0 pone palabra, jugador 1 adivina */
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

            /* REGISTER: registra nuevo usuario si no existe */
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

            /* SETWORD: solo el setter puede registrar la palabra secreta */
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

                    /* Guarda palabra secreta válida y habilita al guesser */
                    if (set_secret_word(&game, msg.arg1))
                    {
                        send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                        send_message(clientes[otro].socket_fd, RESP_YOUR_TURN_GUESS);
                        printf("[HIJO] Jugador %d establecio la palabra secreta\n", i + 1);
                    }
                    else
                    {
                        /* Si la palabra es inválida, el mismo setter la vuelve a intentar */
                        send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_WORD);
                        send_message(clientes[i].socket_fd, RESP_YOUR_TURN_SETWORD);
                        printf("[HIJO] Jugador %d envio una palabra invalida\n", i + 1);
                    }
                }
            }

            /* GUESS: solo el jugador que adivina puede intentar */
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

                    /* Procesa el intento y genera feedback tipo Wordle */
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

                        /* Si la ronda termina, registra el resultado y decide si sigue otra ronda o acaba el juego */
                        if (current_round_over(&game, result))
                        {
                            guessed_correctly = current_round_won(result);

                            register_round_result(&game, game.rm.guesser_id, guessed_correctly);
                            end_round(&game.rm);

                            send_message(clientes[0].socket_fd, RESP_ROUND_END);
                            send_message(clientes[1].socket_fd, RESP_ROUND_END);

                            /* Si ya acabaron las 2 rondas, calcula el resultado final */
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
                                /* Si no ha acabado todo, prepara la siguiente ronda e intercambia roles */
                                prepare_next_round(&game);

                                send_message(clientes[game.rm.setter_id].socket_fd, RESP_YOUR_TURN_SETWORD);
                                send_message(clientes[game.rm.guesser_id].socket_fd, RESP_WAITING_PLAYER);

                                printf("[HIJO] Preparada la ronda %d\n", game.rm.current_round);
                            }
                        }
                        else
                        {
                            /* Si no terminó la ronda, el mismo jugador puede volver a intentar */
                            send_message(clientes[i].socket_fd, RESP_YOUR_TURN_GUESS);
                        }
                    }
                    else
                    {
                        /* Si el intento es inválido, no pierde turno, solo vuelve a intentarlo */
                        send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_WORD);
                        send_message(clientes[i].socket_fd, RESP_YOUR_TURN_GUESS);
                        printf("[HIJO] Jugador %d envio un intento invalido\n", i + 1);
                    }
                }
            }

            /* DISCONNECT: si uno se sale, la sesión se corta */
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

            /* Validaciones de formato para comandos conocidos */
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

            /* Si no coincide con nada, se marca como comando inválido */
            else
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_COMMAND);
                printf("[HIJO] Cliente %d envio comando invalido\n", i + 1);
            }
        }
    }

    /* Al terminar la sesión, el hijo cierra sockets y limpia clientes */
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

    /* Ignora hijos terminados para evitar procesos zombie */
    signal(SIGCHLD, SIG_IGN);

    /* Crea el servidor principal */
    servidor_fd = create_server();
    if (servidor_fd < 0)
    {
        return 1;
    }

    printf("[PADRE] Servidor principal listo en puerto %d\n", PUERTO);

    /* El padre siempre sigue vivo esperando nuevas sesiones */
    while (1)
    {
        /* Reinicia la estructura de clientes para una nueva partida */
        for (i = 0; i < MAX_CLIENTES; i++)
        {
            init_client(&clientes[i]);
        }

        /* Empareja a 2 clientes para formar una sesión */
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

        /* Crea un proceso hijo para manejar esta partida completa */
        pid_t pid = fork();

        if (pid < 0)
        {
            /* Si fork falla, libera sockets de esa sesión y sigue esperando otra */
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
            /* HIJO: cierra el socket del servidor y atiende solo esta sesión */
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