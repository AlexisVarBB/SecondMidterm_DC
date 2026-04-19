#ifndef SESSION_H
#define SESSION_H

#include "client_manager.h"

typedef enum
{
    WAITING_CONNECTIONS,
    WAITING_AUTH,
    READY,
    ACTIVE,
    INTERRUPTED,
    ENDED
} SessionState;

typedef struct
{
    SessionState estado;
} Session;

void init_session(Session *sesion);
void set_session_state(Session *sesion, SessionState nuevo_estado);
int are_both_authenticated(Client clientes[], int total_clientes);

#endif