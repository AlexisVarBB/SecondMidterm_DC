#include <stdio.h>
#include <string.h>
#include "parser.h"

void parse_message(const char *buffer, ParsedMessage *mensaje)
{
    mensaje->comando[0] = '\0';
    mensaje->arg1[0] = '\0';
    mensaje->arg2[0] = '\0';
    mensaje->partes = 0;

    mensaje->partes = sscanf(buffer, "%49s %49s %49s",
                             mensaje->comando,
                             mensaje->arg1,
                             mensaje->arg2);
}