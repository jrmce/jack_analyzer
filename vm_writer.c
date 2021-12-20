#include <stdio.h>
#include <string.h>

FILE *fp_out;

void init_writer(char *filename) {
  fp_out = fopen(strcat(strtok(filename, "."), ".vm"), "w");
}

void close_writer() { fclose(fp_out); }
