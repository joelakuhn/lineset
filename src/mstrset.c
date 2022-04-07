#include <stdlib.h>
#include <string.h>
#include "mstrset.h"

// VECTOR

void mstrset_vec_init(mstrset_vec_t* vec) {
  vec->capacity = STRSET_VEC_STATIC_CAPACITY;
  vec->len = 0;
  vec->items = vec->initial;
}

void mstrset_vec_maybe_reallocate(mstrset_vec_t* vec) {
  if (vec->len >= vec->capacity) {
    size_t new_capacity = vec->capacity + 10;
    mstrset_item_t* new_items = NULL;
    if (vec->len == STRSET_VEC_STATIC_CAPACITY) {
      new_items = malloc(sizeof(mstrset_item_t) * new_capacity);
      memcpy(new_items, vec->initial, sizeof(mstrset_item_t) * STRSET_VEC_STATIC_CAPACITY);
    }
    else {
      new_items = realloc(vec->items, sizeof(mstrset_item_t) * new_capacity);
    }
    if (new_items) {
      vec->items = new_items;
      vec->capacity = new_capacity;
    }
  }
}

void mstrset_vec_push(mstrset_vec_t* vec, char* str, size_t len, mstrset_hash_t hash) {
  mstrset_vec_maybe_reallocate(vec);
  if (vec->len < vec->capacity) {
    vec->items[vec->len].str = str;
    vec->items[vec->len].hash = hash;
    vec->items[vec->len].len = len;
    vec->len++;
  }
}

void mstrset_vec_destroy(mstrset_vec_t* vec) {
  if (vec->len > STRSET_VEC_STATIC_CAPACITY) {
    free(vec->items);
  }
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
  set->strs = malloc(sizeof(mstrset_item_t) * MAX_FILLED);
  set->filled_buckets = malloc(sizeof(size_t) * MAX_FILLED);
  set->filled_buckets_size = 0;
  memset(set->buckets, 0, BUFFER_SIZE);

  return set;
}

mstrset_hash_t mstrset_hash(char* str, size_t len) {
  size_t i = 0;
  size_t stop_64 = len & 0xfffffff8;
  size_t hash_64 = 0;

  for (; i < stop_64; i += 8) {
    uint32_t c = *(uint64_t*)(str + i);
    int shift = i % 64;
    hash_64 ^= (c << shift) | (c >> (shift - 56));
    hash_64 = ~hash_64;
  }

  mstrset_hash_t hash = (hash_64 >> 32) ^ (hash_64 & 0xFFFFFFFF);

  for (; i < len; i++) {
    char c = str[i];
    int shift = i % 32;
    hash ^= c << shift | c >> (shift - 24);
    hash *= c;
    hash = ~hash;
  }

  return hash;
}

void mstrset_resize(mstrset_t* set) {
  set->capacity *= 2;
  set->max_fill *= 2;

  set->strs = realloc(set->strs, sizeof(mstrset_item_t) * set->max_fill);
  size_t new_filled_buckets_size = 0;
  mstrset_bucket_t* new_buckets = malloc(sizeof(mstrset_bucket_t) * set->capacity);
  size_t* new_filled_buckets = malloc(sizeof(size_t) * set->max_fill);
  memset(new_buckets, 0, sizeof(mstrset_bucket_t) * set->capacity);

  for (size_t i = 0; i < set->filled_buckets_size; i++) {
    mstrset_bucket_t* bucket = &(set->buckets[set->filled_buckets[i]]);
    for (size_t j = 0; j < bucket->contents.len; j++) {
      mstrset_item_t* item = &bucket->contents.items[j];
      size_t new_index = item->hash % set->capacity;
      mstrset_bucket_t* new_bucket = &new_buckets[new_index];

      if (!new_bucket->filled) {
        new_bucket->filled = 1;
        mstrset_vec_init(&new_bucket->contents);
        new_filled_buckets[new_filled_buckets_size] = new_index;
        new_filled_buckets_size++;
      }

      mstrset_vec_push(&new_bucket->contents, item->str, item->len, item->hash);
    }
    mstrset_vec_destroy(&bucket->contents);
  }

  free(set->buckets);
  free(set->filled_buckets);
  set->buckets = new_buckets;
  set->filled_buckets = new_filled_buckets;
  set->filled_buckets_size = new_filled_buckets_size;
}

void mstrset_insert(mstrset_t* set, char* str, size_t len) {
  if (set->size >= set->max_fill) {
    mstrset_resize(set);
  }

  mstrset_hash_t hash = mstrset_hash(str, len);
  size_t index = hash % set->capacity;
  mstrset_bucket_t* bucket = &(set->buckets[index]);

  if (!bucket->filled) {
    mstrset_vec_init(&bucket->contents);
    set->filled_buckets[set->filled_buckets_size] = index;
    set->filled_buckets_size++;
    bucket->filled = 1;
  }
  else if (mstrset_bucket_contains(bucket, str, len, hash)) {
    return;
  }
  mstrset_vec_push(&bucket->contents, str, len, hash);
  mstrset_item_t* in_order_slot = set->strs + set->size;
  in_order_slot->str = str;
  in_order_slot->len = len;
  in_order_slot->hash = hash;
  set->size++;
}

int mstrset_bucket_contains(mstrset_bucket_t* bucket, char* str, size_t len, mstrset_hash_t hash) {
  if (bucket->filled) {
    for (size_t i = 0; i < bucket->contents.len; i++) {
      mstrset_item_t item = bucket->contents.items[i];
      if (item.hash == hash) {
        if (item.len == len) {
          if (strcmp(item.str, str) == 0) {
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

int mstrset_contains(mstrset_t* set, char* str) {
  size_t len = strlen(str);
  mstrset_hash_t hash = mstrset_hash(str, len);
  mstrset_bucket_t* bucket = &(set->buckets[hash % set->capacity]);
  return mstrset_bucket_contains(bucket, str, len, hash);
}

void mstrset_destroy(mstrset_t* set) {
  for (size_t i = 0; i < set->filled_buckets_size; i++) {
    mstrset_vec_destroy(&(set->buckets[set->filled_buckets[i]].contents));
  }
  free(set->buckets);
  free(set->filled_buckets);
  free(set->strs);
  free(set);
}
