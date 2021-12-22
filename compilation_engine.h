#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H
#include "token.h"
#include <stdbool.h>
#include <stdio.h>
void compile_class_var_dec(Token *token);
void compile_class(Token *token);
void compile_subroutine_dec(Token *token);
void compile_parameter_list(Token *token);
void compile_subroutine_var_dec(Token *token);
void compile_var_dec(Token *token);
void compile_statements(Token *token);
void compile_let(Token *token);
void compile_if(Token *token);
void compile_while(Token *token);
void compile_do(Token *token);
void compile_return(Token *token);
void compile_expression(Token *token);
void compile_term(Token *token);
int compile_expression_list(Token *token);
void compile_subroutine_call(Token *token);
typedef struct {
  char name[MAX_STRING_LENGTH];
  char return_type[MAX_STRING_LENGTH];
  bool is_constructor;
  bool is_method;
} ActiveFn;
#endif
