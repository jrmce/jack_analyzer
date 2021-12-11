#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

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
  char literal[256];
} Token;

#endif
