#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdbool.h>

void tokenize_file(char *filename);
void advance(void);
void close_tokenizer(void);
bool has_more_tokens(void);
void print_token(void);

#endif
