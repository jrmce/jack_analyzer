#include <ctype.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "constants.h"
#include "tokenizer.h"
#include "util.h"

void process_file(char *filename) {
  tokenize_file(filename);
  advance();

  while (has_more_tokens()) {
    print_token();
    advance();
  }

  close_tokenizer();
}

void process_dir(char *dirname) {
  DIR *dr = opendir(dirname);

  if (dr == NULL) {
    printf("Error opening directory: %s", dirname);
  }

  struct dirent *de;

  while ((de = readdir(dr)) != NULL) {
    if (strstr(de->d_name, ".vm") != NULL) {
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

  if (is_file == NULL) { // Directory
    process_dir(filename);
  } else { // File
    process_file(filename);
  }

  end = clock();
  cpu_time_used = ((double)(end - start)) / (CLOCKS_PER_SEC);
  printf("Analyzer took %f\n", cpu_time_used);

  return 0;
}
