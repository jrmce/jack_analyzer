#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "constants.h"
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
