#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "constants.h"
#include <stdlib.h>

typedef enum { Constant, Local, Arg, Field, Static, Pointer, Temp, That } Kind;
extern const char *SEGMENTS[8];

typedef struct {
  char name[MAX_STRING_LENGTH];
  char type[MAX_STRING_LENGTH];
  Kind kind;
  int segment_index;
} SymbolTableRow;

typedef struct {
  size_t count;
  size_t capacity;
  SymbolTableRow *symbol;
} SymbolTable;

void init_symbol_table(void);
void start_subroutine(void);
void define(SymbolTableRow *symbol);
int var_count(Kind kind);
Kind kind_of(char *name);
char *type_of(char *name);
int index_of(char *name);

#endif
