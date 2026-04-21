#include "session.h"

void init_session(Session *sesion)
{
    sesion->estado = WAITING_CONNECTIONS;
}

void set_session_state(Session *sesion, SessionState nuevo_estado)
{
    sesion->estado = nuevo_estado;
}

int are_both_authenticated(Client clientes[], int total_clientes)
{
    int i;

    for (i = 0; i < total_clientes; i++)
    {
        if (!clientes[i].conectado || !clientes[i].autenticado)
        {
            return 0;
        }
    }

    return 1;
}