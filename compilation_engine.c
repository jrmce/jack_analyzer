#include "constants.h"
#include "parser.h"
#include "symbol_table.h"
#include "token.h"
#include "tokenizer.h"
#include "util.h"
#include "vm_writer.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_type(Token *token);
void eat(Token *token, int count, ...);
void eat_c(Token *token, int count, ...);
void eat_type(Token *token);
void compile_class_var_dec(Token *token);
void compile_class(Token *token);
void compile_subroutine_dec(Token *token);
void compile_parameter_list(Token *token);
void compile_subroutine_in_body(Token *token);
void compile_var_dec(Token *token);
void compile_statements(Token *token);
void compile_let(Token *token);
void compile_if(Token *token);
void compile_while(Token *token);
void compile_do(Token *token);
void compile_return(Token *token);
void compile_expression(Token *token);
void compile_term(Token *token);
void compile_expression_list(Token *token);
void compile_subroutine_call(Token *token);

void eat_keyword(Token *token, int count, ...) {
  va_list argp;
  va_start(argp, count);

  for (int i = 0; i < count; i++) {
    char *expected = va_arg(argp, char *);
    if (strcmp(expected, token->literal) == 0) {
      va_end(argp);
      token = advance();
      return;
    }
  }

  va_end(argp);
  va_start(argp, count);
  for (int i = 0; i < count; i++) {
    char *expected = va_arg(argp, char *);
    printf("Expected keyword not found. %s != %s\n", token->literal, expected);
  }
  exit(1);
}

void eat_token_type(Token *token, int count, ...) {
  va_list argp;
  va_start(argp, count);

  for (int i = 0; i < count; i++) {
    TokenType expected = va_arg(argp, TokenType);
    if (expected == token->type) {
      va_end(argp);
      token = advance();
      return;
    }
  }
  va_end(argp);
  va_start(argp, count);
  for (int i = 0; i < count; i++) {
    TokenType expected = va_arg(argp, TokenType);
    printf("Expected token type not found. %d != %d\n", token->type, expected);
  }
  exit(1);
}

void eat_type(Token *token) {
  if (is_type(token)) {
    token = advance();
  } else {
    printf("Current token is not a type: %s / %d\n", token->literal,
           token->type);
    exit(1);
  }
}

void eat_c(Token *token, int count, ...) {
  va_list argp;
  va_start(argp, count);

  for (int i = 0; i < count; i++) {
    int expected = va_arg(argp, int);
    if (expected == token->literal[0]) {
      token = advance();
      va_end(argp);
      return;
    }
  }
  va_end(argp);
  va_start(argp, count);
  for (int i = 0; i < count; i++) {
    TokenType expected = va_arg(argp, TokenType);
    printf("Expected char not found. %c != %c\n", token->literal[0],
           (char)expected);
  }
  exit(1);
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

void compile_term(Token *token) {
  if (token->type == StringConst || token->type == IntConst ||
      token->type == Keyword) {
    token = advance();
  } else if (token->type == Identifier) {
    char next = (char)peek();
    if (next == LEFT_BRACKET) {
      eat_token_type(token, 1, Identifier);
      eat_c(token, 1, LEFT_BRACKET);
      compile_expression(token);
      eat_c(token, 1, RIGHT_BRACKET);
    } else if ((next == LEFT_PAREN) || (next == PERIOD)) {
      compile_subroutine_call(token);
    } else {
      token = advance();
    }
  } else if (token->type == Symbol) {
    if (token->literal[0] == LEFT_PAREN) {
      eat_c(token, 1, LEFT_PAREN);
      compile_expression(token);
      eat_c(token, 1, RIGHT_PAREN);
    } else if (token->literal[0] == MINUS || token->literal[0] == TILDE) {
      eat_c(token, 2, MINUS, TILDE);
      compile_term(token);
    }
  }
}

void compile_expression(Token *token) {
  compile_term(token);
  while ((token->type == Symbol) &&
         (token->literal[0] == PLUS || token->literal[0] == MINUS ||
          token->literal[0] == ASTERISK || token->literal[0] == SLASH ||
          token->literal[0] == AMPERSAND || token->literal[0] == PIPE ||
          token->literal[0] == LESS_THAN || token->literal[0] == GREATER_THAN ||
          token->literal[0] == EQUAL)) {
    eat_c(token, 9, PLUS, MINUS, ASTERISK, SLASH, AMPERSAND, PIPE, LESS_THAN,
          GREATER_THAN, EQUAL);
    compile_term(token);
  }
}
void compile_expression_list(Token *token) {
  if (token->literal[0] != RIGHT_PAREN) {
    compile_expression(token);
    while ((token->type == Symbol) && (token->literal[0] == COMMA)) {
      eat_c(token, 1, COMMA);
      compile_expression(token);
    }
  }
}

void compile_class_var_dec(Token *token) {
  eat_keyword(token, 2, STATIC, FIELD);
  eat_type(token);
  eat_token_type(token, 1, Identifier);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, 1, COMMA);
    eat_token_type(token, 1, Identifier);
  }
  eat_c(token, 1, SEMICOLON);
}

void compile_parameter_list(Token *token) {
  if (token->literal[0] == RIGHT_PAREN) {
    return;
  }

  eat_type(token);
  eat_token_type(token, 1, Identifier);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, 1, COMMA);
    eat_type(token);
    eat_token_type(token, 1, Identifier);
  }
}

void compile_let(Token *token) {
  eat_keyword(token, 1, LET);
  eat_token_type(token, 1, Identifier);
  if (token->type == Symbol && token->literal[0] == LEFT_BRACKET) {
    eat_c(token, 1, LEFT_BRACKET);
    compile_expression(token);
    eat_c(token, 1, RIGHT_BRACKET);
  }
  eat_c(token, 1, EQUAL);
  compile_expression(token);
  eat_c(token, 1, SEMICOLON);
}

void compile_if(Token *token) {
  eat_keyword(token, 1, IF);
  eat_c(token, 1, LEFT_PAREN);
  compile_expression(token);
  eat_c(token, 1, RIGHT_PAREN);
  eat_c(token, 1, LEFT_BRACE);
  compile_statements(token);
  eat_c(token, 1, RIGHT_BRACE);
  if (strcmp(token->literal, ELSE) == 0) {
    eat_keyword(token, 1, ELSE);
    eat_c(token, 1, LEFT_BRACE);
    compile_statements(token);
    eat_c(token, 1, RIGHT_BRACE);
  }
}

void compile_while(Token *token) {
  eat_keyword(token, 1, WHILE);
  eat_c(token, 1, LEFT_PAREN);
  compile_expression(token);
  eat_c(token, 1, RIGHT_PAREN);
  eat_c(token, 1, LEFT_BRACE);
  compile_statements(token);
  eat_c(token, 1, RIGHT_BRACE);
}

void compile_subroutine_call(Token *token) {
  eat_token_type(token, 1, Identifier);
  if (token->literal[0] == LEFT_PAREN) {
    eat_c(token, 1, LEFT_PAREN);
    compile_expression_list(token);
    eat_c(token, 1, RIGHT_PAREN);
  } else {
    eat_c(token, 1, PERIOD);
    eat_token_type(token, 1, Identifier);
    eat_c(token, 1, LEFT_PAREN);
    compile_expression_list(token);
    eat_c(token, 1, RIGHT_PAREN);
  }
}

void compile_do(Token *token) {
  eat_keyword(token, 1, DO);
  compile_subroutine_call(token);
  eat_c(token, 1, SEMICOLON);
}

void compile_return(Token *token) {
  eat_keyword(token, 1, RETURN);
  if (token->literal[0] != SEMICOLON) {
    compile_expression(token);
  }
  eat_c(token, 1, SEMICOLON);
}

void compile_statements(Token *token) {
  while ((token->type != Symbol) && (token->literal[0] != RIGHT_BRACE)) {
    parse(token);
  }
}

void compile_subroutine_in_body(Token *token) {
  eat_c(token, 1, LEFT_BRACE);
  while ((token->type == Keyword) && (strcmp(token->literal, VAR) == 0)) {
    SymbolTableRow *var = (SymbolTableRow *)malloc(sizeof(SymbolTableRow));
    var->kind = Var;
    eat_keyword(token, 1, VAR);
    strcpy(var->type, token->literal);
    eat_type(token);
    strcpy(var->name, token->literal);
    eat_token_type(token, 1, Identifier);
    define(var);

    while ((token->type == Symbol) && (token->literal[0] == COMMA)) {
      eat_c(token, 1, COMMA);
      strcpy(var->name, token->literal);
      eat_token_type(token, 1, Identifier);
      define(var);
    }
    eat_c(token, 1, SEMICOLON);
  }
  compile_statements(token);
  eat_c(token, 1, RIGHT_BRACE);
}

void compile_subroutine_dec(Token *token) {
  start_subroutine();
  eat_keyword(token, 3, CONSTRUCTOR, FUNCTION, METHOD);
  if ((strcmp(token->literal, VOID) == 0) || is_type(token)) {
    token = advance();
  }
  eat_token_type(token, 1, Identifier);
  eat_c(token, 1, LEFT_PAREN);
  compile_parameter_list(token);
  eat_c(token, 1, RIGHT_PAREN);
  compile_subroutine_in_body(token);
}

void compile_class(Token *token) {
  init_symbol_table();
  eat_keyword(token, 1, CLASS);
  eat_c(token, 1, LEFT_BRACE);
  while ((token->type != Symbol) && (token->literal[0] != RIGHT_BRACE)) {
    parse(token);
  }
  eat_c(token, 1, RIGHT_BRACE);
}
