#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "readfile.h"
#include "mstrset.h"

void fill_set(mstrset_t* set, struct file_contents* file) {
  size_t pos = 0;
  for (size_t i=0; i<file->len; i++) {
    int is_lf = file->data[i] == '\n';
    int is_crlf = !is_lf && (file->data[i] == '\r' && file->data[i + 1] == '\n');

    if (is_lf || is_crlf) {
      file->data[i] = '\0';
      char* line = file->data + pos;
      mstrset_insert(set, line, i - pos);
      if (is_crlf) i++;
      pos = i + 1;
    }
  }
}

int main(int argc, char** argv) {
  int status = 0;

  if (argc == 4) {
    char* file_1_path = argv[1];
    char operator = argv[2][0];
    char* file_2_path = argv[3];

    struct file_contents* file_1 = read_file(file_1_path);
    struct file_contents* file_2 = read_file(file_2_path);

    if (file_1 == NULL || file_2 == NULL) {
      status = 1;
    }

    if (status == 0) {
      mstrset_t* set_1 = mstrset_new();
      mstrset_t* set_2 = mstrset_new();

      fill_set(set_1, file_1);
      fill_set(set_2, file_2);

      if (strcmp(file_1_path, "-") == 0 || strcmp(file_2_path, "-") == 0) {
        printf("Result:\n");
      }

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

      // printf("%zu/%zu/%zu\n", set_1->filled_buckets_size, set_1->size, set_1->capacity);
      // printf("%zu/%zu/%zu\n", set_2->filled_buckets_size, set_2->size, set_2->capacity);

      // size_t max_1 = 0;
      // size_t counts_1[10] = { 0,0,0,0,0,0,0,0,0,0};
      // for (int i=0; i<set_1->filled_buckets_size; i++) {
      //   counts_1[set_1->buckets[set_1->filled_buckets[i]].contents.len]++;
      //   if (set_1->buckets[set_1->filled_buckets[i]].contents.len > max_1) {
      //     max_1 = set_1->buckets[set_1->filled_buckets[i]].contents.len;
      //   }
      // }

      // size_t max_2 = 0;
      // size_t counts_2[10] = { 0,0,0,0,0,0,0,0,0,0};
      // for (int i=0; i<set_2->filled_buckets_size; i++) {
      //   counts_2[set_2->buckets[set_2->filled_buckets[i]].contents.len]++;
      //   if (set_2->buckets[set_2->filled_buckets[i]].contents.len > max_2) {
      //     max_2 = set_2->buckets[set_2->filled_buckets[i]].contents.len;
      //   }
      // }

      // printf("%zu\n", max_1);
      // printf("%zu\n", max_2);

      // for (int i=0; i<10; i++) {
      //   printf("%zu, ", counts_1[i]);
      // }
      // printf("\n");

      // for (int i=0; i<10; i++) {
      //   printf("%zu, ", counts_2[i]);
      // }
      // printf("\n");

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
