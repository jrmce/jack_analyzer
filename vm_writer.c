#include "constants.h"
#include "util.h"
#include <libgen.h>
#include <stdio.h>
#include <string.h>

FILE *fp_out;
char class_name[MAX_STRING_LENGTH];

void init_writer(char *filename) {
  strcpy(class_name, basename(filename));
  strtok(class_name, ".");
  strtok(filename, ".");
  fp_out = fopen(strcat(filename, ".vm"), "w");
}

void close_writer(void) { fclose(fp_out); }

char *get_class_name(void) { return class_name; }

void write_function(char *name, int nlocals) {
  fprintf(fp_out, "function %s.%s %d\n", class_name, name, nlocals);
}

void write_push(const char *segment, int index) {
  fprintf(fp_out, "push %s %d\n", segment, index);
}

void write_pop(const char *segment, int index) {
  fprintf(fp_out, "pop %s %d\n", segment, index);
}

void write_call(char *name, int nargs) {
  fprintf(fp_out, "call %s %d\n", name, nargs);
}

void write_arithmetic(char *command) { fprintf(fp_out, "%s\n", command); }

void write_return() { fprintf(fp_out, "return\n"); }

void write_label(int label_num) { fprintf(fp_out, "label L%d\n", label_num); }

void write_if(int label_num) { fprintf(fp_out, "if-goto L%d\n", label_num); }

void write_goto(int label_num) { fprintf(fp_out, "goto L%d\n", label_num); }
