#include "compilation_engine.h"
#include "constants.h"
#include "token.h"
#include "tokenizer.h"
#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fp_out;

void print_terminal(Token *token);
void parse(Token *token);

void parse(Token *token) {
  switch (token->type) {
  case Keyword:
    if (strcmp(token->literal, CLASS) == 0) {
      compile_class(token, fp_out);
    } else if ((strcmp(token->literal, STATIC) == 0) ||
               strcmp(token->literal, FIELD) == 0) {
      compile_class_var_dec(token, fp_out);
    } else if ((strcmp(token->literal, CONSTRUCTOR) == 0) ||
               (strcmp(token->literal, FUNCTION) == 0) ||
               (strcmp(token->literal, METHOD) == 0)) {
      compile_subroutine_dec(token, fp_out);
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
