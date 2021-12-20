#include "symbol_table.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

static SymbolTable class_sym_table;
static SymbolTable subroutine_sym_table;
static int local_index = 0;
static int arg_index = 0;
static int field_index = 0;
static int static_index = 0;

void define_in(SymbolTable *table, SymbolTableRow *row);

void free_table(SymbolTable *table) {
  free(table->symbol);
  table->count = 0;
  table->capacity = 0;
  table->symbol = NULL;
}

void clear_class_segments() {
  field_index = 0;
  static_index = 0;
}

void clear_subroutine_segments() {
  local_index = 0;
  arg_index = 0;
}

void init_symbol_table(void) {
  free_table(&class_sym_table);
  clear_class_segments();
  free_table(&subroutine_sym_table);
}

void start_subroutine(void) {
  free_table(&subroutine_sym_table);
  clear_subroutine_segments();
}

void define(SymbolTableRow *row) {
  if (row->kind == Static) {
    row->segment_index = static_index;
    define_in(&class_sym_table, row);
    static_index++;
  } else if (row->kind == Field) {
    row->segment_index = field_index;
    define_in(&class_sym_table, row);
    field_index++;
  } else if (row->kind == Arg) {
    row->segment_index = arg_index;
    define_in(&class_sym_table, row);
    arg_index++;
  } else if (row->kind == Var) {
    row->segment_index = local_index;
    define_in(&class_sym_table, row);
    local_index++;
  }
}

void define_in(SymbolTable *table, SymbolTableRow *row) {
  if (table->count < table->count + 1) {
    table->capacity = table->capacity < 8 ? 8 : table->capacity * 2;
    table->symbol = (SymbolTableRow *)realloc(
        table->symbol, table->capacity * sizeof(SymbolTableRow));
  }

  table->symbol[table->count] = *row;
  table->count++;
}
