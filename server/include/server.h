#ifndef SERVER_H
#define SERVER_H

int create_server();
int accept_client(int servidor_fd);
int send_message(int cliente_fd, const char *mensaje);
int receive_message(int cliente_fd, char *buffer, int tam_buffer);

#endif