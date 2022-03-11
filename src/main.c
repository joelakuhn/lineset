#include <stdio.h>
#include <stdint.h>

#include "readfile.h"
#include "mstrset.h"

void fill_set(mstrset_t* set, struct file_contents* file) {
  size_t pos = 0;
  for (size_t i=0; i<file->len; i++) {
    if (file->data[i] == '\n') {
      file->data[i] = '\0';
      char* line = file->data + pos;
      mstrset_insert(set, line, i - pos);
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
      mstrset_t* set_1 = mstrset_new();
      mstrset_t* set_2 = mstrset_new();

      fill_set(set_1, file_1);
      fill_set(set_2, file_2);

      if (operator == '+') {
        for (size_t i=0; i<set_1->size; i++) {
          puts(set_1->strs[i].str);
        }
        for (size_t i=0; i<set_2->size; i++) {
          if (!mstrset_contains(set_1, set_2->strs[i])) {
            puts(set_2->strs[i].str);
          }
        }
      }
      else if (operator == '&') {
        for (size_t i=0; i<set_1->size; i++) {
          if (mstrset_contains(set_2, set_1->strs[i])) {
            puts(set_1->strs[i].str);
          }
        }
      }
      else if (operator == '-') {
        for (size_t i=0; i<set_1->size; i++) {
          if (!mstrset_contains(set_2, set_1->strs[i])) {
            puts(set_1->strs[i].str);
          }
        }
      }
      else if (operator == '^') {
        for (size_t i=0; i<set_1->size; i++) {
          if (!mstrset_contains(set_2, set_1->strs[i])) {
            puts(set_1->strs[i].str);
          }
        }
        for (size_t i=0; i<set_2->size; i++) {
          if (!mstrset_contains(set_1, set_2->strs[i])) {
            puts(set_2->strs[i].str);
          }
        }
      }

      mstrset_destroy(set_1);
      mstrset_destroy(set_2);

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
