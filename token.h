#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

#include "constants.h"

typedef enum {
  Keyword,
  Symbol,
  Identifier,
  IntConst,
  StringConst,
  Unknown
} TokenType;
typedef struct {
  TokenType type;
  char literal[MAX_STRING_LENGTH];
} Token;

#endif
