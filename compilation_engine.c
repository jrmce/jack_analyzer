#include "compilation_engine.h"
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
static int label_count = 0;
static ActiveFn active_fn;

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
  if (token->type == IntConst) {
    write_push(SEG_CONSTANT, atoi(token->literal));
    token = advance();
  } else if (token->type == StringConst) {
    size_t len = strlen(token->literal);
    write_push(SEGMENTS[Constant], (int)len);
    write_call(SYS_STRING_NEW, 1);
    for (size_t i = 0; i < len; i++) {
      write_push(SEGMENTS[Constant], token->literal[i]);
      write_call(SYS_STRING_APPEND, 2);
    }
    token = advance();
  } else if (token->type == Keyword) {
    if (strcmp(token->literal, J_TRUE) == 0) {
      write_push(SEG_CONSTANT, 0);
      write_arithmetic(OP_NOT);
    } else if (strcmp(token->literal, J_FALSE) == 0) {
      write_push(SEG_CONSTANT, 0);
    } else if (strcmp(token->literal, J_NULL) == 0) {
      write_push(SEG_CONSTANT, 0);
    } else if (strcmp(token->literal, THIS) == 0) {
      write_push(SEGMENTS[Pointer], 0);
    }
    token = advance();
  } else if (token->type == Identifier) {
    char next = (char)peek();
    if (next == LEFT_BRACKET) {
      Kind arr_kind = kind_of(token->literal);
      int arr_index = index_of(token->literal);
      eat_token_type(token, 1, Identifier);
      eat_c(token, 1, LEFT_BRACKET);
      compile_expression(token);
      write_push(SEGMENTS[arr_kind], arr_index);
      write_arithmetic(OP_ADD);
      write_pop(SEGMENTS[Pointer], 1);
      eat_c(token, 1, RIGHT_BRACKET);
      write_push(SEGMENTS[That], 0);
    } else if ((next == LEFT_PAREN) || (next == PERIOD)) {
      compile_subroutine_call(token);
    } else {
      write_push(SEGMENTS[kind_of(token->literal)], index_of(token->literal));
      token = advance();
    }
  } else if (token->type == Symbol) {
    if (token->literal[0] == LEFT_PAREN) {
      eat_c(token, 1, LEFT_PAREN);
      compile_expression(token);
      eat_c(token, 1, RIGHT_PAREN);
    } else if (token->literal[0] == MINUS) {
      eat_c(token, 2, MINUS, TILDE);
      compile_term(token);
      write_arithmetic(OP_NEG);
    } else if (token->literal[0] == TILDE) {
      eat_c(token, 2, MINUS, TILDE);
      compile_term(token);
      write_arithmetic(OP_NOT);
    }
  }
}

char *get_arithmetic_name(char symbol) {
  switch (symbol) {
  case PLUS:
    return OP_ADD;
  case MINUS:
    return OP_SUB;
  case ASTERISK:
    return SYS_MULTIPLY;
  case SLASH:
    return SYS_DIVIDE;
  case AMPERSAND:
    return OP_AND;
  case PIPE:
    return OP_OR;
  case LESS_THAN:
    return OP_LT;
  case GREATER_THAN:
    return OP_GT;
  case EQUAL:
    return OP_EQ;
  default:
    return "";
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
    char *sym_text = get_arithmetic_name(token->literal[0]);

    eat_c(token, 9, PLUS, MINUS, ASTERISK, SLASH, AMPERSAND, PIPE, LESS_THAN,
          GREATER_THAN, EQUAL);

    compile_term(token);

    if (strcmp(sym_text, SYS_MULTIPLY) == 0 ||
        strcmp(sym_text, SYS_DIVIDE) == 0) {
      write_call(sym_text, 2);
    } else {
      write_arithmetic(sym_text);
    }
  }
}

int compile_expression_list(Token *token) {
  int nargs = 0;
  if (token->literal[0] != RIGHT_PAREN) {
    nargs++;
    compile_expression(token);
    while ((token->type == Symbol) && (token->literal[0] == COMMA)) {
      eat_c(token, 1, COMMA);
      compile_expression(token);
      nargs++;
    }
  }
  return nargs;
}

void compile_class_var_dec(Token *token) {
  SymbolTableRow *class_dec = (SymbolTableRow *)malloc(sizeof(SymbolTableRow));
  class_dec->kind = strcmp(token->literal, STATIC) == 0 ? Static : Field;
  eat_keyword(token, 2, STATIC, FIELD);
  strcpy(class_dec->type, token->literal);
  eat_type(token);
  strcpy(class_dec->name, token->literal);
  eat_token_type(token, 1, Identifier);
  define(class_dec);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, 1, COMMA);
    strcpy(class_dec->name, token->literal);
    eat_token_type(token, 1, Identifier);
    define(class_dec);
  }
  eat_c(token, 1, SEMICOLON);
  free(class_dec);
}

void compile_parameter_list(Token *token) {
  if (token->literal[0] == RIGHT_PAREN) {
    return;
  }

  SymbolTableRow *arg = (SymbolTableRow *)malloc(sizeof(SymbolTableRow));
  arg->kind = Arg;
  strcpy(arg->type, token->literal);
  eat_type(token);
  strcpy(arg->name, token->literal);
  eat_token_type(token, 1, Identifier);
  define(arg);

  while (token->type == Symbol && token->literal[0] == COMMA) {
    eat_c(token, 1, COMMA);
    strcpy(arg->type, token->literal);
    eat_type(token);
    strcpy(arg->name, token->literal);
    eat_token_type(token, 1, Identifier);
    define(arg);
  }

  free(arg);
}

void compile_let(Token *token) {
  char var_name[MAX_STRING_LENGTH];

  eat_keyword(token, 1, LET);
  strcpy(var_name, token->literal);
  Kind kind = kind_of(var_name);
  int index = index_of(var_name);
  eat_token_type(token, 1, Identifier);
  if (token->type == Symbol && token->literal[0] == LEFT_BRACKET) {
    eat_c(token, 1, LEFT_BRACKET);
    compile_expression(token);
    write_push(SEGMENTS[kind], index);
    write_arithmetic(OP_ADD);
    eat_c(token, 1, RIGHT_BRACKET);
    eat_c(token, 1, EQUAL);
    compile_expression(token);
    write_pop(SEGMENTS[Temp], 0);
    write_pop(SEGMENTS[Pointer], 1);
    write_push(SEGMENTS[Temp], 0);
    eat_c(token, 1, SEMICOLON);
    write_pop(SEGMENTS[That], 0);
  } else {
    eat_c(token, 1, EQUAL);
    compile_expression(token);
    eat_c(token, 1, SEMICOLON);
    write_pop(SEGMENTS[kind], index);
  }
}

void compile_if(Token *token) {
  int label_1 = label_count++;
  int label_2 = label_count++;
  eat_keyword(token, 1, IF);
  eat_c(token, 1, LEFT_PAREN);
  compile_expression(token);
  write_arithmetic(OP_NOT);
  write_if(label_1);
  eat_c(token, 1, RIGHT_PAREN);
  eat_c(token, 1, LEFT_BRACE);
  compile_statements(token);
  eat_c(token, 1, RIGHT_BRACE);
  if (strcmp(token->literal, ELSE) == 0) {
    write_goto(label_2);
    write_label(label_1);
    eat_keyword(token, 1, ELSE);
    eat_c(token, 1, LEFT_BRACE);
    compile_statements(token);
    eat_c(token, 1, RIGHT_BRACE);
    write_label(label_2);
  } else {
    write_label(label_1);
  }
}

void compile_while(Token *token) {
  int label_1 = label_count++;
  int label_2 = label_count++;
  write_label(label_1);
  eat_keyword(token, 1, WHILE);
  eat_c(token, 1, LEFT_PAREN);
  compile_expression(token);
  write_arithmetic(OP_NOT);
  write_if(label_2);
  eat_c(token, 1, RIGHT_PAREN);
  eat_c(token, 1, LEFT_BRACE);
  compile_statements(token);
  write_goto(label_1);
  eat_c(token, 1, RIGHT_BRACE);
  write_label(label_2);
}

void compile_subroutine_call(Token *token) {
  char identifier[MAX_STRING_LENGTH];
  char class_name[MAX_STRING_LENGTH];
  int nargs = 0;

  strcpy(class_name, get_class_name());
  strcpy(identifier, token->literal);
  eat_token_type(token, 1, Identifier);

  if (token->literal[0] == LEFT_PAREN) {
    strcat(class_name, ".");
    strcat(class_name, identifier);
    strcpy(identifier, class_name);
    eat_c(token, 1, LEFT_PAREN);
    write_push(SEGMENTS[Pointer], 0);
    nargs++;
    nargs += compile_expression_list(token);
    eat_c(token, 1, RIGHT_PAREN);
  } else {
    char *type = type_of(identifier);

    if (type !=
        NULL) { // This is a method call and not a constructor/function call
      Kind kind = kind_of(identifier);
      int index = index_of(identifier);

      // Push the this to the stack as first arg
      write_push(SEGMENTS[kind], index);
      nargs++;

      strcpy(identifier, type);
    }

    strcat(identifier, ".");
    eat_c(token, 1, PERIOD);
    strcat(identifier, token->literal);
    eat_token_type(token, 1, Identifier);
    eat_c(token, 1, LEFT_PAREN);
    nargs += compile_expression_list(token);
    eat_c(token, 1, RIGHT_PAREN);
  }
  write_call(identifier, nargs);
}

void compile_do(Token *token) {
  eat_keyword(token, 1, DO);
  compile_subroutine_call(token);
  write_pop(SEG_TEMP, 0);
  eat_c(token, 1, SEMICOLON);
}

void compile_return(Token *token) {
  eat_keyword(token, 1, RETURN);

  if (token->literal[0] != SEMICOLON) {
    compile_expression(token);
  } else if (strcmp(active_fn.return_type, VOID) == 0) {
    write_push(SEG_CONSTANT, 0);
  } else if (active_fn.is_constructor) {
    write_push(SEGMENTS[Pointer], 0);
  }

  write_return();
  eat_c(token, 1, SEMICOLON);
}

void compile_statements(Token *token) {
  while ((token->type != Symbol) && (token->literal[0] != RIGHT_BRACE)) {
    parse(token);
  }
}

void compile_subroutine_var_dec(Token *token) {
  while ((token->type == Keyword) && (strcmp(token->literal, VAR) == 0)) {
    SymbolTableRow *var = (SymbolTableRow *)malloc(sizeof(SymbolTableRow));
    var->kind = Local;
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
    free(var);
  }
}

void define_this() {
  SymbolTableRow *this = (SymbolTableRow *)malloc(sizeof(SymbolTableRow));
  strcpy(this->name, THIS);
  strcpy(this->type, get_class_name());
  this->kind = Arg;
  define(this);
}

void compile_subroutine_dec(Token *token) {
  active_fn.is_constructor = false;
  active_fn.is_method = false;

  start_subroutine();

  if (strcmp(token->literal, METHOD) == 0) {
    define_this();
    active_fn.is_method = true;
  }

  if (strcmp(token->literal, CONSTRUCTOR) == 0) {
    active_fn.is_constructor = true;
  }

  eat_keyword(token, 3, CONSTRUCTOR, FUNCTION, METHOD);

  if ((strcmp(token->literal, VOID) == 0) || is_type(token)) {
    strcpy(active_fn.return_type, token->literal);
    token = advance();
  }

  strcpy(active_fn.name, token->literal);
  eat_token_type(token, 1, Identifier);

  eat_c(token, 1, LEFT_PAREN);
  compile_parameter_list(token);
  eat_c(token, 1, RIGHT_PAREN);

  eat_c(token, 1, LEFT_BRACE);
  compile_subroutine_var_dec(token);

  write_function(active_fn.name, var_count(Local));

  if (active_fn.is_constructor) {
    int words = var_count(Field);
    write_push(SEGMENTS[Constant], words);
    write_call(SYS_ALLOC, 1);
    write_pop(SEGMENTS[Pointer], 0);
  } else if (active_fn.is_method) {
    write_push(SEG_ARGUMENT, 0);
    write_pop(SEG_POINTER, 0);
  }

  compile_statements(token);
  eat_c(token, 1, RIGHT_BRACE);
}

void compile_class(Token *token) {
  init_symbol_table();
  eat_keyword(token, 1, CLASS);
  token = advance();
  eat_c(token, 1, LEFT_BRACE);
  while ((token->type != Symbol) && (token->literal[0] != RIGHT_BRACE)) {
    parse(token);
  }
  eat_c(token, 1, RIGHT_BRACE);
}
