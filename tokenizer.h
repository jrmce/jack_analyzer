#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "token.h"
#include <stdbool.h>

void tokenize_file(char *filename);
void close_tokenizer(void);
bool has_more_tokens(void);
Token *advance(void);
int peek();

#endif
