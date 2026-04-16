#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server.h"
#include "protocol.h"

int main()
{
    int servidor_fd;
    int clientes[MAX_CLIENTES];
    char buffer[BUFFER_SIZE];
    int i;

    servidor_fd = create_server();
    if (servidor_fd < 0)
    {
        return 1;
    }

    for (i = 0; i < MAX_CLIENTES; i++)
    {
        printf("Esperando al cliente %d...\n", i + 1);
        clientes[i] = accept_client(servidor_fd);

        if (clientes[i] < 0)
        {
            close(servidor_fd);
            return 1;
        }

        if (i == 0)
        {
            send_message(clientes[i], "WELCOME_CLIENT_1");
        }
        else
        {
            send_message(clientes[i], "WELCOME_CLIENT_2");
        }
    }

    printf("\nLos dos clientes ya están conectados\n");

    for (i = 0; i < MAX_CLIENTES; i++)
    {
        int leidos = receive_message(clientes[i], buffer, BUFFER_SIZE);

        if (leidos > 0)
        {
            printf("Mensaje del cliente %d: %s\n", i + 1, buffer);
            send_message(clientes[i], "MESSAGE_RECEIVED");
        }
    }

    for (i = 0; i < MAX_CLIENTES; i++)
    {
        close(clientes[i]);
    }

    close(servidor_fd);
    printf("Servidor finalizado\n");

    return 0;
}