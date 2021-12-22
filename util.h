#ifndef util_h
#define util_h

#include "token.h"
#include <stdbool.h>
#include <stdio.h>

extern const char *OPERATIONS[9];
void get_filename_no_ext(char *filename, char *dest);
bool is_block_comment(char c, char p);
bool is_line_comment(char c, char p);
bool is_keyword(Token *token);
bool is_symbol(char c);
bool is_ascii(char c);

#endif
