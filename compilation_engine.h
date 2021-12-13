#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "token.h"

void init_parser(char *filename);
void parse(Token *token);
void close_parser(void);

#endif
