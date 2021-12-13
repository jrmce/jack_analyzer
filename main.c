#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "compilation_engine.h"
#include "constants.h"
#include "tokenizer.h"
#include "util.h"

void process_file(char *filename) {
  tokenize_file(filename);
  init_parser(filename);
  parse(advance());
  close_tokenizer();
  close_parser();
}

void process_dir(char *dirname) {
  DIR *dr = opendir(dirname);

  if (dr == NULL) {
    printf("Error opening directory: %s", dirname);
  }

  struct dirent *de;

  while ((de = readdir(dr)) != NULL) {
    if (strstr(de->d_name, ".jack") != NULL) {
      char dirname_c[MAX_FILENAME_LENGTH];
      strcpy(dirname_c, dirname);
      strcat(dirname_c, "/");
      strcat(dirname_c, de->d_name);
      process_file(dirname_c);
    }
  }

  closedir(dr);
}

int main(int argc, char *argv[]) {
  clock_t start;
  clock_t end;
  double cpu_time_used;

  start = clock();

  if (argc < 2) {
    exit(1);
  }

  char *filename = argv[1];

  char *is_file = strstr(filename, ".jack");

  if (is_file == NULL) {
    process_dir(filename);
  } else {
    process_file(filename);
  }

  end = clock();
  cpu_time_used = ((double)(end - start)) / (CLOCKS_PER_SEC);
  printf("Analyzer took %f\n", cpu_time_used);

  return 0;
}
