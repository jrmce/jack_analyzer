#include "constants.h"
#include "parser.h"
#include "token.h"
#include "tokenizer.h"
#include "util.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_type(Token *token);
void eat(Token *token, int count, ...);
void eat_c(Token *token, char expected);
void eat_type(Token *token);
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

void eat_keyword(Token *token, int count, ...) {
  va_list argp;
  va_start(argp, count);

  for (int i = 0; i < count; i++) {
    char *expected = va_arg(argp, char *);
    if (strcmp(expected, token->literal) == 0) {
      print_terminal(token);
      break;
    }
  }

  va_end(argp);
}

void eat_token_type(Token *token, int count, ...) {
  va_list argp;
  va_start(argp, count);

  for (int i = 0; i < count; i++) {
    TokenType expected = va_arg(argp, TokenType);
    if (expected == token->type) {
      print_terminal(token);
      break;
    }
  }

  va_end(argp);
}

void eat_type(Token *token) {
  if (is_type(token)) {
    print_terminal(token);
  } else {
    printf("Current token is not a type: %s / %d\n", token->literal,
           token->type);
    exit(1);
  }
}

void eat_c(Token *token, char expected) {
  if (expected == token->literal[0]) {
    print_terminal(token);
  } else {
    printf("Current token does not match expected: %c != %c\n",
           token->literal[0], expected);
    exit(1);
  }
}

bool is_type(Token *token) {
  if (((token->type == Keyword) && ((strcmp(token->literal, INT) == 0) ||
                                    (strcmp(token->literal, CHAR) == 0) ||
                                    (strcmp(token->literal, BOOLEAN) == 0))) ||
      (token->type == Identifier)) {
    return true;
  }
  return false;
}

void compile_class_var_dec(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<classVarDec>\n");
  eat_keyword(token, 2, STATIC, FIELD);
  eat_type(token);
  eat_token_type(token, 1, Identifier);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, COMMA);
    eat_token_type(token, 1, Identifier);
  }
  eat_c(token, SEMICOLON);
  fprintf(fp_out, "</classVarDec>\n");
}

void compile_parameter_list(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<parameterList>\n");
  if (token->literal[0] == RIGHT_PAREN) {
    fprintf(fp_out, "</parameterList>\n");
    return;
  }

  eat_type(token);
  eat_token_type(token, 1, Identifier);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, COMMA);
    eat_type(token);
    eat_token_type(token, 1, Identifier);
  }
  fprintf(fp_out, "</parameterList>\n");
}

void compile_subroutine_in_body(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<subroutineBody>\n");
  eat_c(token, LEFT_BRACE);
  eat_c(token, RIGHT_BRACE);
  fprintf(fp_out, "</subroutineBody>\n");
}

void compile_subroutine_dec(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<subroutineDec>\n");
  eat_keyword(token, 3, CONSTRUCTOR, FUNCTION, METHOD);
  if ((strcmp(token->literal, VOID) == 0) || is_type(token)) {
    print_terminal(token);
  }
  eat_token_type(token, 1, Identifier);
  eat_c(token, LEFT_PAREN);
  compile_parameter_list(token, fp_out);
  eat_c(token, RIGHT_PAREN);
  compile_subroutine_in_body(token, fp_out);
  fprintf(fp_out, "</subroutineDec>\n");
}

void compile_class(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<class>\n");
  eat_keyword(token, 1, CLASS);
  print_terminal(token);
  eat_c(token, LEFT_BRACE);
  while (token->type != Symbol && token->literal[0] != RIGHT_BRACE) {
    parse(token);
  }
  eat_c(token, RIGHT_BRACE);
  fprintf(fp_out, "</class>\n");
}
