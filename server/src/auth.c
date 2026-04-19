#include <stdio.h>
#include <string.h>
#include "auth.h"

int validate_credentials(const char *ruta_archivo, const char *usuario, const char *password)
{
    FILE *archivo;
    char usuario_archivo[MAX_USER];
    char password_archivo[MAX_PASS];

    archivo = fopen(ruta_archivo, "r");
    if (archivo == NULL)
    {
        return -1;
    }

    while (fscanf(archivo, "%49s %49s", usuario_archivo, password_archivo) == 2)
    {
        if (strcmp(usuario_archivo, usuario) == 0 &&
            strcmp(password_archivo, password) == 0)
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int user_exists(const char *ruta_archivo, const char *usuario)
{
    FILE *archivo;
    char usuario_archivo[MAX_USER];
    char password_archivo[MAX_PASS];

    archivo = fopen(ruta_archivo, "r");
    if (archivo == NULL)
    {
        return -1;
    }

    while (fscanf(archivo, "%49s %49s", usuario_archivo, password_archivo) == 2)
    {
        if (strcmp(usuario_archivo, usuario) == 0)
        {
            fclose(archivo);
            return 1;
        }
    }

    fclose(archivo);
    return 0;
}

int register_user(const char *ruta_archivo, const char *usuario, const char *password)
{
    FILE *archivo;

    archivo = fopen(ruta_archivo, "a");
    if (archivo == NULL)
    {
        return 0;
    }

    fprintf(archivo, "%s %s\n", usuario, password);
    fclose(archivo);

    return 1;
}