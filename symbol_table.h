#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <stdlib.h>

typedef enum { Static, Field, Arg, Var } Kind;
typedef struct {
  char name[256];
  char type[256];
  Kind kind;
  int segment_index;
} SymbolTableRow;

typedef struct {
  int count;
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
