#include "symbol_table.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SymbolTable class_sym_table;
static SymbolTable subroutine_sym_table;
static int local_index = 0;
static int arg_index = 0;
static int field_index = 0;
static int static_index = 0;
const char *SEGMENTS[8] = {SEG_CONSTANT, SEG_LOCAL,   SEG_ARGUMENT, SEG_FIELD,
                           SEG_STATIC,   SEG_POINTER, SEG_TEMP,     SEG_THAT};

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
}

void start_subroutine(void) {
  free_table(&subroutine_sym_table);
  clear_subroutine_segments();
}

int var_count(Kind kind) {
  int count = 0;

  if (kind == Local || kind == Arg) {
    for (size_t i = 0; i < subroutine_sym_table.count; i++) {
      if (subroutine_sym_table.symbol[i].kind == kind) {
        count++;
      }
    }
  } else if (kind == Field || kind == Static) {
    for (size_t i = 0; i < class_sym_table.count; i++) {
      if (class_sym_table.symbol[i].kind == kind) {
        count++;
      }
    }
  }

  return count;
}

Kind kind_of(char *name) {
  for (size_t i = 0; i < subroutine_sym_table.count; i++) {
    if (strcmp(subroutine_sym_table.symbol[i].name, name) == 0) {
      return subroutine_sym_table.symbol[i].kind;
    }
  }

  for (size_t i = 0; i < class_sym_table.count; i++) {
    if (strcmp(class_sym_table.symbol[i].name, name) == 0) {
      return class_sym_table.symbol[i].kind;
    }
  }

  printf("Unable to find expected variable kind: %s\n", name);
  exit(1);
}

int index_of(char *name) {
  for (size_t i = 0; i < subroutine_sym_table.count; i++) {
    if (strcmp(subroutine_sym_table.symbol[i].name, name) == 0) {
      return subroutine_sym_table.symbol[i].segment_index;
    }
  }

  for (size_t i = 0; i < class_sym_table.count; i++) {
    if (strcmp(class_sym_table.symbol[i].name, name) == 0) {
      return class_sym_table.symbol[i].segment_index;
    }
  }

  printf("Unable to find expected variable index: %s\n", name);
  exit(1);
}

char *type_of(char *name) {
  for (size_t i = 0; i < subroutine_sym_table.count; i++) {
    if (strcmp(subroutine_sym_table.symbol[i].name, name) == 0) {
      return subroutine_sym_table.symbol[i].type;
    }
  }

  for (size_t i = 0; i < class_sym_table.count; i++) {
    if (strcmp(class_sym_table.symbol[i].name, name) == 0) {
      return class_sym_table.symbol[i].type;
    }
  }

  printf("Unable to find expected variable type: %s\n", name);
  return NULL;
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
    define_in(&subroutine_sym_table, row);
    arg_index++;
  } else if (row->kind == Local) {
    row->segment_index = local_index;
    define_in(&subroutine_sym_table, row);
    local_index++;
  }
}

void define_in(SymbolTable *table, SymbolTableRow *row) {
  if (table->capacity < table->count + 1) {
    table->capacity = table->capacity < 8 ? 8 : table->capacity * 2;
    table->symbol = (SymbolTableRow *)realloc(
        table->symbol, table->capacity * sizeof(SymbolTableRow));
  }

  table->symbol[table->count] = *row;
  table->count++;
}
