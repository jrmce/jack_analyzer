#include "tokenizer.h"
#include "constants.h"
#include "token.h"
#include "util.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Token current_token;
static int current_char;
static bool has_more_chars = true;
static FILE *fp;
static FILE *fp_out;

void next_char() {
  if ((current_char = fgetc(fp)) != EOF) {
    has_more_chars = true;
  } else {
    has_more_chars = false;
  }
}

int peek_char() {
  int c = fgetc(fp);
  ungetc(c, fp);
  return c;
}

bool is_comment() {
  if (current_char == '/' && peek_char() == '/') {
    return true;
  }

  return false;
}

void set_token_type() {
  if (current_token.literal[0] == '\0') {
    printf("Invalid token");
    exit(1);
  } else if (strcmp(CLASS, current_token.literal) == 0 ||
             strcmp(CONSTRUCTOR, current_token.literal) == 0 ||
             strcmp(FUNCTION, current_token.literal) == 0 ||
             strcmp(METHOD, current_token.literal) == 0 ||
             strcmp(FIELD, current_token.literal) == 0 ||
             strcmp(STATIC, current_token.literal) == 0 ||
             strcmp(VAR, current_token.literal) == 0 ||
             strcmp(INT, current_token.literal) == 0 ||
             strcmp(CHAR, current_token.literal) == 0 ||
             strcmp(BOOLEAN, current_token.literal) == 0 ||
             strcmp(VOID, current_token.literal) == 0 ||
             strcmp(J_TRUE, current_token.literal) == 0 ||
             strcmp(J_FALSE, current_token.literal) == 0 ||
             strcmp(J_NULL, current_token.literal) == 0 ||
             strcmp(THIS, current_token.literal) == 0 ||
             strcmp(LET, current_token.literal) == 0 ||
             strcmp(DO, current_token.literal) == 0 ||
             strcmp(IF, current_token.literal) == 0 ||
             strcmp(ELSE, current_token.literal) == 0 ||
             strcmp(WHILE, current_token.literal) == 0 ||
             strcmp(RETURN, current_token.literal) == 0) {
    current_token.type = Keyword;
  }
}

void print_token() {
  if (current_token.type == Keyword) {
    fprintf(fp_out, "<keyword>%s</keyword>\n", current_token.literal);
  }
}

void handle_comment() {
  while (current_char != '\n') {
    next_char();
  }
}

void handle_alpha_or_underscore() {
  int p = 0;
  while (!isspace(current_char)) {
    current_token.literal[p] = (char)current_char;
    p++;
    next_char();
  }

  current_token.literal[p] = '\0';
  set_token_type();
}

void reset_token() {
  current_token.literal[0] = '\0';
  current_token.type = Unknown;
}

void tokenize_file(char *filename) {
  fp = fopen(filename, "r");
  char filename_no_ext[MAX_FILENAME_LENGTH];

  if (fp == NULL) {
    printf("Error opening source file: %s\n", filename);
    exit(1);
  }

  get_filename_no_ext(filename, filename_no_ext);

  fp_out = fopen(strcat(filename_no_ext, ".xml"), "w");
  fputs("<tokens>\n", fp_out);
  next_char();
}

void advance() {
  reset_token();

  if (is_comment()) {
    handle_comment();
  } else if (isalpha(current_char) || current_char == '_') {
    handle_alpha_or_underscore();
  }

  next_char();
}

bool has_more_tokens() { return has_more_chars; }

void close_tokenizer() {
  fputs("</tokens>\n", fp_out);
  fclose(fp_out);
  fclose(fp);
}
