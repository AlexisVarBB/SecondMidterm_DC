#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "server.h"
#include "protocol.h"

int create_server()
{
    int servidor_fd;
    struct sockaddr_in direccion;
    int opt = 1;

    servidor_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_fd < 0)
    {
        perror("Error al crear el socket");
        return -1;
    }

    if (setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Error en setsockopt");
        close(servidor_fd);
        return -1;
    }

    memset(&direccion, 0, sizeof(direccion));
    direccion.sin_family = AF_INET;
    direccion.sin_addr.s_addr = INADDR_ANY;
    direccion.sin_port = htons(PUERTO);

    if (bind(servidor_fd, (struct sockaddr *)&direccion, sizeof(direccion)) < 0)
    {
        perror("Error en bind");
        close(servidor_fd);
        return -1;
    }

    if (listen(servidor_fd, MAX_CLIENTES) < 0)
    {
        perror("Error en listen");
        close(servidor_fd);
        return -1;
    }

    printf("Servidor escuchando en el puerto %d\n", PUERTO);
    return servidor_fd;
}

int accept_client(int servidor_fd)
{
    int cliente_fd;
    struct sockaddr_in cliente_dir;
    socklen_t largo = sizeof(cliente_dir);

    cliente_fd = accept(servidor_fd, (struct sockaddr *)&cliente_dir, &largo);
    if (cliente_fd < 0)
    {
        perror("Error al aceptar cliente");
        return -1;
    }

    printf("Cliente conectado desde %s:%d\n",
           inet_ntoa(cliente_dir.sin_addr),
           ntohs(cliente_dir.sin_port));

    return cliente_fd;
}

int send_message(int cliente_fd, const char *mensaje)
{
    int enviados = send(cliente_fd, mensaje, strlen(mensaje), 0);
    if (enviados < 0)
    {
        perror("Error al enviar mensaje");
        return -1;
    }
    return enviados;
}

int receive_message(int cliente_fd, char *buffer, int tam_buffer)
{
    int recibidos;

    memset(buffer, 0, tam_buffer);
    recibidos = recv(cliente_fd, buffer, tam_buffer - 1, 0);

    if (recibidos < 0)
    {
        perror("Error al recibir mensaje");
        return -1;
    }

    if (recibidos == 0)
    {
        printf("El cliente cerró la conexión\n");
        return 0;
    }

    buffer[recibidos] = '\0';
    return recibidos;
}