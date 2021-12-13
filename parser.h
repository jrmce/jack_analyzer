#ifndef PARSER_H
#define PARSER_H

#include "token.h"

void init_parser(char *filename);
void parse(Token *token);
void close_parser(void);
void print_terminal(Token *token);

#endif
