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
  if (has_more_chars) {
    int c = fgetc(fp);
    ungetc(c, fp);
    return c;
  }

  return false;
}

void set_token_type() {
  if (current_token.literal[0] == '\0') {
    printf("Invalid token");
    exit(1);
  } else if (is_keyword(&current_token)) {
    current_token.type = Keyword;
  } else {
    current_token.type = Identifier;
  }
}

void handle_line_comment() {
  while (current_char != '\n') {
    next_char();
  }
  // Consume \n
  next_char();
}

void handle_block_comment() {
  while (true) {
    if (current_char == '*' && peek_char() == '/') {
      next_char();
      next_char();
      return;
    }
    next_char();
  }
}

void handle_alpha_or_underscore() {
  int p = 0;
  while (isalnum(current_char) || current_char == '_') {
    current_token.literal[p] = (char)current_char;
    p++;
    next_char();
  }

  current_token.literal[p] = '\0';
  set_token_type();
}

void handle_digit() {
  int p = 0;

  while (isdigit(current_char)) {
    current_token.literal[p] = (char)current_char;
    p++;
    next_char();
  }

  current_token.literal[p] = '\0';
  current_token.type = IntConst;
}

void handle_double_quote() {
  int p = 0;
  next_char();

  while (is_ascii((char)current_char) && current_char != '"') {
    if (current_char == '\n') {
      printf("New line in unterminated string: %s\n", current_token.literal);
      exit(1);
    }

    current_token.literal[p] = (char)current_char;
    p++;
    next_char();
  }

  if (current_char == '"') {
    current_token.literal[p] = '\0';
    current_token.type = StringConst;
    next_char();
  } else {
    printf("Unterminated string constant: %s\n", current_token.literal);
    exit(1);
  }
}

void handle_symbol() {
  current_token.literal[0] = (char)current_char;
  current_token.literal[1] = '\0';
  current_token.type = Symbol;
  next_char();
}

void handle_ignored() {
  if (is_line_comment((char)current_char, (char)peek_char())) {
    handle_line_comment();
  } else if (is_block_comment((char)current_char, (char)peek_char())) {
    handle_block_comment();
  } else if (isspace(current_char)) {
    next_char();
  }

  return;
}

bool ignore() {
  return is_line_comment((char)current_char, (char)peek_char()) ||
         is_block_comment((char)current_char, (char)peek_char()) ||
         isspace(current_char);
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

  while (ignore()) {
    handle_ignored();
  }

  if (isalpha(current_char) || current_char == '_') {
    handle_alpha_or_underscore();
  } else if (current_char == '"') {
    handle_double_quote();
  } else if (isdigit(current_char)) {
    handle_digit();
  } else if (is_symbol((char)current_char)) {
    handle_symbol();
  }

  return;
}

bool has_more_tokens() { return has_more_chars; }

void print_token() {
  if (current_token.type == Keyword) {
    fprintf(fp_out, "<keyword> %s </keyword>\n", current_token.literal);
  } else if (current_token.type == StringConst) {
    fprintf(fp_out, "<stringConstant> %s </stringConstant>\n",
            current_token.literal);
  } else if (current_token.type == IntConst) {
    fprintf(fp_out, "<integerConstant> %s </integerConstant>\n",
            current_token.literal);
  } else if (current_token.type == Identifier) {
    fprintf(fp_out, "<identifier> %s </identifier>\n", current_token.literal);
  } else if (current_token.type == Symbol) {
    if (current_token.literal[0] == LESS_THAN) {
      fprintf(fp_out, "<symbol> &lt; </symbol>\n");
    } else if (current_token.literal[0] == GREATER_THAN) {
      fprintf(fp_out, "<symbol> &gt; </symbol>\n");
    } else if (current_token.literal[0] == '"') {
      fprintf(fp_out, "<symbol> &quot; </symbol>\n");
    } else if (current_token.literal[0] == AMPERSAND) {
      fprintf(fp_out, "<symbol> &amp; </symbol>\n");
    } else {
      fprintf(fp_out, "<symbol> %s </symbol>\n", current_token.literal);
    }
  }
}

void close_tokenizer() {
  fputs("</tokens>\n", fp_out);
  fclose(fp_out);
  fclose(fp);
}
