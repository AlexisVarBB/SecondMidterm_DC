#ifndef AUTH_H
#define AUTH_H

#define MAX_USER 50
#define MAX_PASS 50

int validate_credentials(const char *ruta_archivo, const char *usuario, const char *password);
int user_exists(const char *ruta_archivo, const char *usuario);
int register_user(const char *ruta_archivo, const char *usuario, const char *password);

#endif