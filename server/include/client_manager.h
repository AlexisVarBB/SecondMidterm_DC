#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#define MAX_USERNAME 50

typedef struct
{
    int socket_fd;
    int conectado;
    int autenticado;
    char username[MAX_USERNAME];
} Client;

void init_client(Client *cliente);
void set_client_connected(Client *cliente, int socket_fd);
void set_client_authenticated(Client *cliente, const char *username);
void reset_client(Client *cliente);

#endif