#include <stdlib.h>
#include <string.h>
#include "mstrset.h"

// VECTOR

void mstrset_vec_init(mstrset_vec_t* mstrset_vec) {
  mstrset_vec->items = malloc(sizeof(mstrset_vec_item_t) * 10);
  mstrset_vec->capacity = 10;
  mstrset_vec->len = 0;
}

void mstrset_vec_maybe_reallocate(mstrset_vec_t* mstrset_vec) {
  if (mstrset_vec->len >= mstrset_vec->capacity) {
    size_t new_capacity = mstrset_vec->capacity + 10;
    mstrset_vec_item_t* new_items = realloc(mstrset_vec->items, sizeof(mstrset_vec_item_t) * new_capacity);
    if (new_items) {
      mstrset_vec->items = new_items;
      mstrset_vec->capacity = new_capacity;
    }
  }
}

void mstrset_vec_push(mstrset_vec_t* mstrset_vec, char* str, mstrset_hash_t hash) {
  mstrset_vec_maybe_reallocate(mstrset_vec);
  if (mstrset_vec->len < mstrset_vec->capacity) {
    mstrset_vec->items[mstrset_vec->len].str = str;
    mstrset_vec->items[mstrset_vec->len].hash = hash;
    mstrset_vec->len++;
  }
}

void mstrset_vec_destroy(mstrset_vec_t* mstrset_vec) {
  free(mstrset_vec->items);
  free(mstrset_vec);
}

// SET

mstrset_t* mstrset_new() {
  static const size_t INITIAL_CAPCITY = 16384;
  // static const size_t INITIAL_CAPCITY = 2;
  static const size_t MAX_FILLED = INITIAL_CAPCITY / 2;
  static const size_t BUFFER_SIZE = sizeof(mstrset_bucket_t) * INITIAL_CAPCITY;

  mstrset_t* set = malloc(sizeof(mstrset_t));
  set->size = 0;
  set->capacity = INITIAL_CAPCITY;
  set->buckets = malloc(BUFFER_SIZE);
  set->max_fill = MAX_FILLED;
  set->strs = malloc(sizeof(char*) * MAX_FILLED);
  set->filled_buckets = malloc(sizeof(size_t) * MAX_FILLED);
  set->filled_buckets_size = 0;
  memset(set->buckets, 0, BUFFER_SIZE);

  return set;
}

mstrset_hash_t mstrset_hash(char* str, size_t len) {
  mstrset_hash_t hash = 0b01010101010101010101010101010101;

  for (size_t i = 0; i < len; i++) {
    int shift = (i * 13) % 32;
    hash ^= (str[i] << shift) | (str[i] >> (shift - 24));
    hash *= str[i];
    hash = ~hash;
  }

  return hash;
}

void mstrset_resize(mstrset_t* set) {
  set->capacity *= 2;
  set->max_fill *= 2;

  set->strs = realloc(set->strs, sizeof(char*) * set->max_fill);
  size_t new_filled_buckets_size = 0;
  mstrset_bucket_t* new_buckets = malloc(sizeof(mstrset_bucket_t) * set->capacity);
  size_t* new_filled_buckets = malloc(sizeof(size_t) * set->max_fill);
  memset(new_buckets, 0, sizeof(mstrset_bucket_t) * set->capacity);

  for (size_t i = 0; i < set->filled_buckets_size; i++) {
    mstrset_bucket_t* bucket = &(set->buckets[set->filled_buckets[i]]);
    for (size_t j = 0; j < bucket->contents.len; j++) {
      mstrset_vec_item_t* item = &bucket->contents.items[j];
      size_t new_index = item->hash % set->capacity;
      mstrset_bucket_t* new_bucket = &new_buckets[new_index];

      if (!new_bucket->filled) {
        new_bucket->filled = 1;
        mstrset_vec_init(&new_bucket->contents);
        new_filled_buckets[new_filled_buckets_size] = new_index;
        new_filled_buckets_size++;
      }

      mstrset_vec_push(&new_bucket->contents, item->str, item->hash);
    }
  }

  free(set->buckets);
  free(set->filled_buckets);
  set->buckets = new_buckets;
  set->filled_buckets = new_filled_buckets;
  set->filled_buckets_size = new_filled_buckets_size;
}

void mstrset_insert(mstrset_t* set, char* str) {
  if (set->size >= set->max_fill) {
    mstrset_resize(set);
  }

  mstrset_hash_t hash = mstrset_hash(str, strlen(str));
  size_t index = hash % set->capacity;
  mstrset_bucket_t* bucket = &(set->buckets[index]);

  if (!bucket->filled) {
    mstrset_vec_init(&bucket->contents);
    set->filled_buckets[set->filled_buckets_size] = index;
    set->filled_buckets_size++;
    bucket->filled = 1;
  }
  else if (mstrset_bucket_contains(bucket, str, hash)) {
    return;
  }
  mstrset_vec_push(&bucket->contents, str, hash);
  set->strs[set->size] = str;
  set->size++;
}

int mstrset_bucket_contains(mstrset_bucket_t* bucket, char* str, mstrset_hash_t hash) {
  if (bucket->filled) {
    for (size_t i = 0; i < bucket->contents.len; i++) {
      if (bucket->contents.items[i].hash == hash) {
        if (strcmp(bucket->contents.items[i].str, str) == 0) {
          return 1;
        }
      }
    }
  }
  return 0;
}

int mstrset_contains(mstrset_t* set, char* str) {
  mstrset_hash_t hash = mstrset_hash(str, strlen(str));
  mstrset_bucket_t* bucket = &(set->buckets[hash % set->capacity]);
  return mstrset_bucket_contains(bucket, str, hash);
}

void mstrset_destroy(mstrset_t* set) {
  // free(set->buckets);
  // free(set);
}

// int main() {
//   char* str1 = "hey";
//   char* str2 = "there";
//   char* str3 = "delilah";
//   char* str4 = "A man — A man and nothing more. Yet he presumed to wage war with a god.";
  
//   printf("%i\n", mstrset_hash(str1, strlen(str1)));
//   printf("%i\n", mstrset_hash(str2, strlen(str2)));
//   printf("%i\n", mstrset_hash(str3, strlen(str3)));
//   printf("%i\n", mstrset_hash(str4, strlen(str4)));

//   mstrset_t* set = mstrset_new();
//   mstrset_insert(set, str1);
//   mstrset_insert(set, str2);
//   mstrset_insert(set, str3);

//   printf("%s: %i\n", str1, mstrset_contains(set, str1));
//   printf("%s: %i\n", str2, mstrset_contains(set, str2));
//   printf("%s: %i\n", str3, mstrset_contains(set, str3));
//   printf("%s: %i\n", "hey-oh", mstrset_contains(set, "hey-oh"));
//   printf("%s: %i\n", "printf", mstrset_contains(set, "printf"));

//   printf("final bucket count: %zu\n", set->filled_buckets_size);
//   printf("final size: %zu\n", set->size);
//   printf("final capacity: %zu\n", set->capacity);

//   for (size_t i = 0; i < set->size; i++) {
//     printf("%s\n", set->strs[i]);
//   }

//   return 0;
// }
