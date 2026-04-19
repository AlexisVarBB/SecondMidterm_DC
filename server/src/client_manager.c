#include <string.h>
#include "client_manager.h"

void init_client(Client *cliente)
{
    cliente->socket_fd = -1;
    cliente->conectado = 0;
    cliente->autenticado = 0;
    cliente->username[0] = '\0';
}

void set_client_connected(Client *cliente, int socket_fd)
{
    cliente->socket_fd = socket_fd;
    cliente->conectado = 1;
    cliente->autenticado = 0;
    cliente->username[0] = '\0';
}

void set_client_authenticated(Client *cliente, const char *username)
{
    cliente->autenticado = 1;
    strncpy(cliente->username, username, MAX_USERNAME - 1);
    cliente->username[MAX_USERNAME - 1] = '\0';
}

void reset_client(Client *cliente)
{
    cliente->socket_fd = -1;
    cliente->conectado = 0;
    cliente->autenticado = 0;
    cliente->username[0] = '\0';
}