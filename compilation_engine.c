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
void eat_c(Token *token, int count, ...);
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
void compile_subroutine_call(Token *token, FILE *fp_out);

void eat_keyword(Token *token, int count, ...) {
  va_list argp;
  va_start(argp, count);

  for (int i = 0; i < count; i++) {
    char *expected = va_arg(argp, char *);
    if (strcmp(expected, token->literal) == 0) {
      print_terminal(token);
      va_end(argp);
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
      print_terminal(token);
      va_end(argp);
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
    print_terminal(token);
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
      print_terminal(token);
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

void compile_term(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<term>\n");
  if (token->type == StringConst || token->type == IntConst ||
      token->type == Keyword) {
    print_terminal(token);
  } else if (token->type == Identifier) {
    char next = (char)peek();
    if (next == LEFT_BRACKET) {
      eat_token_type(token, 1, Identifier);
      eat_c(token, 1, LEFT_BRACKET);
      compile_expression(token, fp_out);
      eat_c(token, 1, RIGHT_BRACKET);
    } else if ((next == LEFT_PAREN) || (next == PERIOD)) {
      compile_subroutine_call(token, fp_out);
    } else {
      print_terminal(token);
    }
  } else if (token->type == Symbol) {
    if (token->literal[0] == LEFT_PAREN) {
      eat_c(token, 1, LEFT_PAREN);
      compile_expression(token, fp_out);
      eat_c(token, 1, RIGHT_PAREN);
    } else if (token->literal[0] == MINUS || token->literal[0] == TILDE) {
      eat_c(token, 2, MINUS, TILDE);
      compile_term(token, fp_out);
    }
  }
  fprintf(fp_out, "</term>\n");
}

void compile_expression(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<expression>\n");
  compile_term(token, fp_out);
  while ((token->type == Symbol) &&
         (token->literal[0] == PLUS || token->literal[0] == MINUS ||
          token->literal[0] == ASTERISK || token->literal[0] == SLASH ||
          token->literal[0] == AMPERSAND || token->literal[0] == PIPE ||
          token->literal[0] == LESS_THAN || token->literal[0] == GREATER_THAN ||
          token->literal[0] == EQUAL)) {
    eat_c(token, 9, PLUS, MINUS, ASTERISK, SLASH, AMPERSAND, PIPE, LESS_THAN,
          GREATER_THAN, EQUAL);
    compile_term(token, fp_out);
  }
  fprintf(fp_out, "</expression>\n");
}
void compile_expression_list(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<expressionList>\n");
  if (token->literal[0] != RIGHT_PAREN) {
    compile_expression(token, fp_out);
    while ((token->type == Symbol) && (token->literal[0] == COMMA)) {
      eat_c(token, 1, COMMA);
      compile_expression(token, fp_out);
    }
  }
  fprintf(fp_out, "</expressionList>\n");
}

void compile_class_var_dec(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<classVarDec>\n");
  eat_keyword(token, 2, STATIC, FIELD);
  eat_type(token);
  eat_token_type(token, 1, Identifier);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, 1, COMMA);
    eat_token_type(token, 1, Identifier);
  }
  eat_c(token, 1, SEMICOLON);
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
    eat_c(token, 1, COMMA);
    eat_type(token);
    eat_token_type(token, 1, Identifier);
  }
  fprintf(fp_out, "</parameterList>\n");
}

void compile_let(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<letStatement>\n");
  eat_keyword(token, 1, LET);
  eat_token_type(token, 1, Identifier);
  if (token->type == Symbol && token->literal[0] == LEFT_BRACKET) {
    eat_c(token, 1, LEFT_BRACKET);
    compile_expression(token, fp_out);
    eat_c(token, 1, RIGHT_BRACKET);
  }
  eat_c(token, 1, EQUAL);
  compile_expression(token, fp_out);
  eat_c(token, 1, SEMICOLON);
  fprintf(fp_out, "</letStatement>\n");
}

void compile_if(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<ifStatement>\n");
  eat_keyword(token, 1, IF);
  eat_c(token, 1, LEFT_PAREN);
  compile_expression(token, fp_out);
  eat_c(token, 1, RIGHT_PAREN);
  eat_c(token, 1, LEFT_BRACE);
  compile_statements(token, fp_out);
  eat_c(token, 1, RIGHT_BRACE);
  if (strcmp(token->literal, "else") == 0) {
    eat_keyword(token, 1, ELSE);
    eat_c(token, 1, LEFT_BRACE);
    compile_statements(token, fp_out);
    eat_c(token, 1, RIGHT_BRACE);
  }
  fprintf(fp_out, "</ifStatement>\n");
}

void compile_while(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<whileStatement>\n");
  eat_keyword(token, 1, WHILE);
  eat_c(token, 1, LEFT_PAREN);
  compile_expression(token, fp_out);
  eat_c(token, 1, RIGHT_PAREN);
  eat_c(token, 1, LEFT_BRACE);
  compile_statements(token, fp_out);
  eat_c(token, 1, RIGHT_BRACE);
  fprintf(fp_out, "</whileStatement>\n");
}

void compile_subroutine_call(Token *token, FILE *fp_out) {
  eat_token_type(token, 1, Identifier);
  if (token->literal[0] == LEFT_PAREN) {
    eat_c(token, 1, LEFT_PAREN);
    compile_expression_list(token, fp_out);
    eat_c(token, 1, RIGHT_PAREN);
  } else {
    eat_c(token, 1, PERIOD);
    eat_token_type(token, 1, Identifier);
    eat_c(token, 1, LEFT_PAREN);
    compile_expression_list(token, fp_out);
    eat_c(token, 1, RIGHT_PAREN);
  }
}

void compile_do(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<doStatement>\n");
  eat_keyword(token, 1, DO);
  compile_subroutine_call(token, fp_out);
  eat_c(token, 1, SEMICOLON);
  fprintf(fp_out, "</doStatement>\n");
}

void compile_return(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<returnStatement>\n");
  eat_keyword(token, 1, RETURN);
  if (token->literal[0] != SEMICOLON) {
    compile_expression(token, fp_out);
  }
  eat_c(token, 1, SEMICOLON);
  fprintf(fp_out, "</returnStatement>\n");
}

void compile_statements(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<statements>\n");
  while ((token->type != Symbol) && (token->literal[0] != RIGHT_BRACE)) {
    parse(token);
  }
  fprintf(fp_out, "</statements>\n");
}

void compile_subroutine_in_body(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<subroutineBody>\n");
  eat_c(token, 1, LEFT_BRACE);
  while ((token->type == Keyword) && (strcmp(token->literal, VAR) == 0)) {
    fprintf(fp_out, "<varDec>\n");
    eat_keyword(token, 1, VAR);
    eat_type(token);
    eat_token_type(token, 1, Identifier);

    while ((token->type == Symbol) && (token->literal[0] == COMMA)) {
      eat_c(token, 1, COMMA);
      eat_token_type(token, 1, Identifier);
    }
    eat_c(token, 1, SEMICOLON);
    fprintf(fp_out, "</varDec>\n");
  }
  compile_statements(token, fp_out);
  eat_c(token, 1, RIGHT_BRACE);
  fprintf(fp_out, "</subroutineBody>\n");
}

void compile_subroutine_dec(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<subroutineDec>\n");
  eat_keyword(token, 3, CONSTRUCTOR, FUNCTION, METHOD);
  if ((strcmp(token->literal, VOID) == 0) || is_type(token)) {
    print_terminal(token);
  }
  eat_token_type(token, 1, Identifier);
  eat_c(token, 1, LEFT_PAREN);
  compile_parameter_list(token, fp_out);
  eat_c(token, 1, RIGHT_PAREN);
  compile_subroutine_in_body(token, fp_out);
  fprintf(fp_out, "</subroutineDec>\n");
}

void compile_class(Token *token, FILE *fp_out) {
  fprintf(fp_out, "<class>\n");
  eat_keyword(token, 1, CLASS);
  print_terminal(token);
  eat_c(token, 1, LEFT_BRACE);
  while ((token->type != Symbol) && (token->literal[0] != RIGHT_BRACE)) {
    parse(token);
  }
  eat_c(token, 1, RIGHT_BRACE);
  fprintf(fp_out, "</class>\n");
}
