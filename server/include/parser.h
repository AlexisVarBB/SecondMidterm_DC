#ifndef PARSER_H
#define PARSER_H

#define MAX_TOKEN 50

typedef struct
{
    char comando[MAX_TOKEN];
    char arg1[MAX_TOKEN];
    char arg2[MAX_TOKEN];
    int partes;
} ParsedMessage;

void parse_message(const char *buffer, ParsedMessage *mensaje);

#endif