#include "compilation_engine.h"
#include "constants.h"
#include "token.h"
#include "tokenizer.h"
#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse(Token *token) {
  switch (token->type) {
  case Keyword:
    if (strcmp(token->literal, CLASS) == 0) {
      compile_class(token);
    } else if ((strcmp(token->literal, STATIC) == 0) ||
               strcmp(token->literal, FIELD) == 0) {
      compile_class_var_dec(token);
    } else if ((strcmp(token->literal, CONSTRUCTOR) == 0) ||
               (strcmp(token->literal, FUNCTION) == 0) ||
               (strcmp(token->literal, METHOD) == 0)) {
      compile_subroutine_dec(token);
    } else if (strcmp(token->literal, LET) == 0) {
      compile_let(token);
    } else if (strcmp(token->literal, IF) == 0) {
      compile_if(token);
    } else if (strcmp(token->literal, WHILE) == 0) {
      compile_while(token);
    } else if (strcmp(token->literal, DO) == 0) {
      compile_do(token);
    } else if (strcmp(token->literal, RETURN) == 0) {
      compile_return(token);
    }
    break;
  case Identifier:
    break;
  case StringConst:
    break;
  case IntConst:
    break;
  case Symbol:
    break;
  default:
    break;
  }
}
