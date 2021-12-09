#include <stdio.h>
#include <stdint.h>

#include "readfile.h"
#include "strset.h"

void fill_set(strset_t* set, struct file_contents* file) {
  size_t pos = 0;
  for (size_t i=0; i<file->len; i++) {
    if (file->data[i] == '\n') {
      file->data[i] = '\0';
      char* line = file->data + pos;
      if (!strset_contains(set, line)) {
        strset_insert(set, line);
      }
      pos = i + 1;
    }
  }
}

int main(int argc, char** argv) {
  int status = 0;

  if (argc == 4) {
    struct file_contents* file_1 = read_file(argv[1]);
    char operator = argv[2][0];
    struct file_contents* file_2 = read_file(argv[3]);

    if (file_1 == NULL || file_2 == NULL) {
      status = 1;
    }

    if (status == 0) {
      strset_t* set_1 = strset_new();
      strset_t* set_2 = strset_new();

      fill_set(set_1, file_1);
      fill_set(set_2, file_2);

      if (operator == '+') {
        for (size_t i=0; i<set_1->count; i++) {
          puts(set_1->lines[i]);
        }
        for (size_t i=0; i<set_2->count; i++) {
          if (!strset_contains(set_1, set_2->lines[i])) {
            puts(set_2->lines[i]);
          }
        }
      }
      else if (operator == '&') {
        for (size_t i=0; i<set_1->count; i++) {
          if (strset_contains(set_2, set_1->lines[i])) {
            puts(set_1->lines[i]);
          }
        }
      }
      else if (operator == '-') {
        for (size_t i=0; i<set_1->count; i++) {
          if (!strset_contains(set_2, set_1->lines[i])) {
            puts(set_1->lines[i]);
          }
        }
      }
      else if (operator == '^') {
        for (size_t i=0; i<set_1->count; i++) {
          if (!strset_contains(set_2, set_1->lines[i])) {
            puts(set_1->lines[i]);
          }
        }
        for (size_t i=0; i<set_2->count; i++) {
          if (!strset_contains(set_1, set_2->lines[i])) {
            puts(set_2->lines[i]);
          }
        }
      }
      strset_destroy(set_1);
      strset_destroy(set_2);

      free_file(file_1);
      free_file(file_2);
    }
  }
  else {
    puts("Usage: lineset file1 operator file2");
    puts("");
    puts("Operators:");
    puts("  +  union");
    puts("  &  intersection");
    puts("  -  difference");
    puts("  ^  symmetric difference");
  }

  return status;
}
