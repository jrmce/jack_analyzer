#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H
#include "token.h"
#include <stdio.h>
void compile_class_var_dec(Token *token, FILE *fp_out);
void compile_class(Token *token, FILE *fp_out);
void compile_subroutine_dec(Token *token, FILE *fp_out);
void compile_parameter_list(Token *token, FILE *fp_out);
void compile_subroutine_in_body(Token *token, FILE *fp_out);
void compile_var_dec(Token *token, FILE *fp_out);
void compile_statements(Token *token, FILE *fp_out);
void compile_let(Token *token, FILE *fp_out);
void compile_if(Token *token, FILE *fp_out);
void compile_while(Token *token, FILE *fp_out);
void compile_do(Token *token, FILE *fp_out);
void compile_return(Token *token, FILE *fp_out);
void compile_expression(Token *token, FILE *fp_out);
void compile_term(Token *token, FILE *fp_out);
void compile_expression_list(Token *token, FILE *fp_out);
void compile_subroutine_call(Token *token, FILE *fp_out);
#endif
