#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "constants.h"
#include "token.h"
#include "util.h"

bool is_dir(const char *path) {
  struct stat statbuf;
  if (stat(path, &statbuf) != 0) {
    printf("Error reading source: %s", path);
    exit(1);
  }
  return S_ISDIR(statbuf.st_mode);
}

void get_filename_no_ext(char *filename, char *dest) {
  int i = 0;
  while (filename[i] != '/') {
    if (filename[i] == '.') {
      i = -1;
      break;
    }
    i++;
  }

  strncpy(dest, &filename[i + 1], MAX_FILENAME_LENGTH);
  strtok(dest, ".");
}

bool is_ascii(char c) { return c > 0 && c < 127; }

bool is_symbol(char c) {
  return c == LEFT_BRACE || c == RIGHT_BRACE || c == LEFT_PAREN ||
         c == RIGHT_PAREN || c == LEFT_BRACKET || c == RIGHT_BRACKET ||
         c == PERIOD || c == COMMA || c == SEMICOLON || c == PLUS ||
         c == MINUS || c == ASTERISK || c == SLASH || c == AMPERSAND ||
         c == PIPE || c == LESS_THAN || c == GREATER_THAN || c == EQUAL ||
         c == TILDE || c == UNDERSCORE;
}

bool is_keyword(Token *token) {
  return strcmp(CLASS, (char *)token->literal) == 0 ||
         strcmp(CONSTRUCTOR, token->literal) == 0 ||
         strcmp(FUNCTION, token->literal) == 0 ||
         strcmp(METHOD, token->literal) == 0 ||
         strcmp(FIELD, token->literal) == 0 ||
         strcmp(STATIC, token->literal) == 0 ||
         strcmp(VAR, token->literal) == 0 || strcmp(INT, token->literal) == 0 ||
         strcmp(CHAR, token->literal) == 0 ||
         strcmp(BOOLEAN, token->literal) == 0 ||
         strcmp(VOID, token->literal) == 0 ||
         strcmp(J_TRUE, token->literal) == 0 ||
         strcmp(J_FALSE, token->literal) == 0 ||
         strcmp(J_NULL, token->literal) == 0 ||
         strcmp(THIS, token->literal) == 0 ||
         strcmp(LET, token->literal) == 0 || strcmp(DO, token->literal) == 0 ||
         strcmp(IF, token->literal) == 0 || strcmp(ELSE, token->literal) == 0 ||
         strcmp(WHILE, token->literal) == 0 ||
         strcmp(RETURN, token->literal) == 0;
}

bool is_line_comment(char c, char p) {
  if (c == '/' && p == '/') {
    return true;
  }

  return false;
}

bool is_block_comment(char c, char p) {
  if (c == '/' && p == '*') {
    return true;
  }

  return false;
}
