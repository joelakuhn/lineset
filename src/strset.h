#ifndef STRSET_H
#define STRSET_H

#include <stdint.h>

#include "readfile.h"
#include "../hashmap.c/hashmap.h"

typedef struct strset {
  struct hashmap* map;
  char** lines;
  size_t count;
  size_t buffer_size;
} strset_t;

strset_t* strset_new(struct file_contents* file);
bool strset_contains(strset_t* set, char* line);
void strset_insert(strset_t* set, char* line);
void strset_destroy(strset_t* set);

#endif
