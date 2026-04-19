#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "server.h"
#include "protocol.h"
#include "config.h"
#include "auth.h"
#include "parser.h"
#include "client_manager.h"
#include "session.h"

int main()
{
    int servidor_fd;
    Client clientes[MAX_CLIENTES];
    Session sesion;
    char buffer[BUFFER_SIZE];
    int i;
    int sesion_activa = 1;

    servidor_fd = create_server();
    if (servidor_fd < 0)
    {
        return 1;
    }

    init_session(&sesion);

    for (i = 0; i < MAX_CLIENTES; i++)
    {
        init_client(&clientes[i]);
    }

    for (i = 0; i < MAX_CLIENTES; i++)
    {
        int socket_cliente;

        printf("Esperando al cliente %d...\n", i + 1);
        socket_cliente = accept_client(servidor_fd);

        if (socket_cliente < 0)
        {
            close(servidor_fd);
            return 1;
        }

        set_client_connected(&clientes[i], socket_cliente);
    }

    set_session_state(&sesion, WAITING_AUTH);

    printf("\nLos dos clientes ya están conectados\n");

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

            leidos = receive_message(clientes[i].socket_fd, buffer, BUFFER_SIZE);

            if (leidos == 0)
            {
                int otro = (i == 0) ? 1 : 0;

                printf("Cliente %d se desconecto\n", i + 1);
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
                printf("Error al recibir mensaje del cliente %d\n", i + 1);
                set_session_state(&sesion, INTERRUPTED);
                sesion_activa = 0;
                break;
            }

            printf("Mensaje del cliente %d: %s\n", i + 1, buffer);

            parse_message(buffer, &msg);

            if (msg.partes == 3 && strcmp(msg.comando, CMD_LOGIN) == 0)
            {
                resultado_auth = validate_credentials(RUTA_USERS, msg.arg1, msg.arg2);

                if (resultado_auth == 1)
                {
                    set_client_authenticated(&clientes[i], msg.arg1);
                    send_message(clientes[i].socket_fd, RESP_LOGIN_OK);
                    printf("Cliente %d autenticado como %s\n", i + 1, clientes[i].username);

                    if (are_both_authenticated(clientes, MAX_CLIENTES))
                    {
                        set_session_state(&sesion, READY);
                        send_message(clientes[0].socket_fd, RESP_START_GAME);
                        send_message(clientes[1].socket_fd, RESP_START_GAME);
                        printf("Ambos clientes autenticados. Sesion lista para iniciar\n");
                    }
                    else
                    {
                        send_message(clientes[i].socket_fd, RESP_WAITING_PLAYER);
                    }
                }
                else if (resultado_auth == 0)
                {
                    send_message(clientes[i].socket_fd, RESP_LOGIN_FAIL);
                    printf("Cliente %d fallo autenticacion\n", i + 1);
                }
                else
                {
                    send_message(clientes[i].socket_fd, RESP_LOGIN_FAIL);
                    printf("Error al abrir archivo de credenciales\n");
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
                    printf("Cliente %d intento registrar un usuario existente: %s\n", i + 1, msg.arg1);
                }
                else if (existe == 0)
                {
                    registrado = register_user(RUTA_USERS, msg.arg1, msg.arg2);

                    if (registrado == 1)
                    {
                        send_message(clientes[i].socket_fd, RESP_REGISTER_OK);
                        printf("Cliente %d registro al usuario %s correctamente\n", i + 1, msg.arg1);
                    }
                    else
                    {
                        send_message(clientes[i].socket_fd, RESP_REGISTER_FAIL);
                        printf("Error al registrar usuario %s\n", msg.arg1);
                    }
                }
                else
                {
                    send_message(clientes[i].socket_fd, RESP_REGISTER_FAIL);
                    printf("Error al abrir archivo de credenciales\n");
                }
            }
            else if (msg.partes == 1 && strcmp(msg.comando, CMD_DISCONNECT) == 0)
            {
                int otro = (i == 0) ? 1 : 0;

                printf("Cliente %d solicito desconexion\n", i + 1);
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
                printf("Cliente %d envio LOGIN con formato invalido\n", i + 1);
            }
            else if (msg.partes >= 1 && strcmp(msg.comando, CMD_REGISTER) == 0)
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_FORMAT);
                printf("Cliente %d envio REGISTER con formato invalido\n", i + 1);
            }
            else
            {
                send_message(clientes[i].socket_fd, RESP_ERROR_INVALID_COMMAND);
                printf("Cliente %d envio comando invalido\n", i + 1);
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

    close(servidor_fd);
    printf("Servidor finalizado\n");

    return 0;
}