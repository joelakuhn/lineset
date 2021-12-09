#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../hashmap.c/hashmap.h"
#include "readfile.h"
#include "strset.h"

const int STRSET_BUFFER_INCREMENT = 1048576;

int strset_str_compare(const void *a, const void *b, void *udata) {
  return strcmp((char*)a, (char*)b);
}

uint64_t strset_hash(const void *item, uint64_t seed0, uint64_t seed1) {
  char* str = (char*)item;
  return hashmap_sip(str, strlen(str), seed0, seed1);
}


strset_t* strset_new(struct file_contents* file) {
  strset_t* set = malloc(sizeof(strset_t));
  set->map = hashmap_new(sizeof(char*), 0, 0, 0, strset_hash, strset_str_compare, NULL, NULL);
  set->lines = malloc(sizeof(char*) * STRSET_BUFFER_INCREMENT);
  set->count = 0;
  set->buffer_size = STRSET_BUFFER_INCREMENT;

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

  return set;
}

void strset_insert(strset_t* set, char* line) {
  if (set->count >= set->buffer_size) {
    set->buffer_size += STRSET_BUFFER_INCREMENT;
    set->lines = realloc(set->lines, set->buffer_size);
  }
  set->lines[set->count++] = line;
  hashmap_set(set->map, line);
}

bool strset_contains(strset_t* set, char* line) {
  return hashmap_get(set->map, line) != NULL;
}

void strset_destroy(strset_t* set) {
  hashmap_free(set->map);
  free(set->lines);
  free(set);
}
