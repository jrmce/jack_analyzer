
#include "constants.h"
#include "token.h"
#include "tokenizer.h"
#include "util.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp_out;

bool is_type(Token *token);
void print_terminal(Token *token);
void eat(Token *token, int count, ...);
void eat_c(Token *token, char expected);
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
void parse(Token *token);

void print_terminal(Token *token) {
  if (token->type == Keyword) {
    fprintf(fp_out, "<keyword> %s </keyword>\n", token->literal);
  } else if (token->type == StringConst) {
    fprintf(fp_out, "<stringConstant> %s </stringConstant>\n", token->literal);
  } else if (token->type == IntConst) {
    fprintf(fp_out, "<integerConstant> %s </integerConstant>\n",
            token->literal);
  } else if (token->type == Identifier) {
    fprintf(fp_out, "<identifier> %s </identifier>\n", token->literal);
  } else if (token->type == Symbol) {
    if (token->literal[0] == LESS_THAN) {
      fprintf(fp_out, "<symbol> &lt; </symbol>\n");
    } else if (token->literal[0] == GREATER_THAN) {
      fprintf(fp_out, "<symbol> &gt; </symbol>\n");
    } else if (token->literal[0] == '"') {
      fprintf(fp_out, "<symbol> &quot; </symbol>\n");
    } else if (token->literal[0] == AMPERSAND) {
      fprintf(fp_out, "<symbol> &amp; </symbol>\n");
    } else {
      fprintf(fp_out, "<symbol> %s </symbol>\n", token->literal);
    }
  }

  token = advance();
}

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
  if ((strcmp(token->literal, INT) == 0) ||
      (strcmp(token->literal, CHAR) == 0) ||
      (strcmp(token->literal, BOOLEAN) == 0) || (token->type == Identifier)) {
    return true;
  }
  return false;
}

void compile_class_var_dec(Token *token) {
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

void compile_parameter_list(Token *token) {
  eat_type(token);
  eat_token_type(token, 1, Identifier);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, COMMA);
    eat_type(token);
    eat_token_type(token, 1, Identifier);
  }
}

void compile_subroutine_in_body(Token *token) {
  fprintf(fp_out, "<subroutineBody>\n");
  eat_c(token, LEFT_BRACE);
  eat_c(token, RIGHT_BRACE);
  fprintf(fp_out, "</subroutineBody>\n");
}

void compile_subroutine_dec(Token *token) {
  fprintf(fp_out, "<subroutineDec>\n");
  eat_keyword(token, 3, CONSTRUCTOR, FUNCTION, METHOD);
  if ((strcmp(token->literal, VOID) == 0) || is_type(token)) {
    print_terminal(token);
  }
  eat_token_type(token, 1, Identifier);
  eat_c(token, LEFT_PAREN);
  if (token->type != Symbol) {
    compile_parameter_list(token);
  }
  eat_c(token, RIGHT_PAREN);
  compile_subroutine_in_body(token);
  fprintf(fp_out, "</subroutineDec>\n");
}

void compile_class(Token *token) {
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

void init_parser(char *filename) {
  char filename_no_ext[MAX_FILENAME_LENGTH];
  get_filename_no_ext(filename, filename_no_ext);
  fp_out = fopen(strcat(filename_no_ext, ".xml"), "w");
}

void close_parser() { fclose(fp_out); }
