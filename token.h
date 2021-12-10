#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

typedef enum { Keyword, Symbol, IntConst, StringConst, Unknown } TokenType;
typedef struct {
  TokenType type;
  char literal[256];
} Token;

#endif
