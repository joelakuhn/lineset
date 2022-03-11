#ifndef MSTRSET_H
#define MSTRSET_H

#include <stdint.h>
#include <stdio.h>

typedef uint32_t mstrset_hash_t;

#define STRSET_VEC_STATIC_CAPACITY 2

typedef struct mstrset_item {
    char* str;
    mstrset_hash_t hash;
    size_t len;
} mstrset_item_t;

typedef struct mstrset_vec {
    mstrset_item_t* items;
    mstrset_item_t initial[STRSET_VEC_STATIC_CAPACITY];
    int len;
    int capacity;
} mstrset_vec_t;

typedef struct mstrset_bucket {
  mstrset_vec_t contents;
  uint8_t filled;
} mstrset_bucket_t;

typedef struct mstrset {
  size_t* filled_buckets;
  size_t filled_buckets_size;
  size_t capacity;
  size_t max_fill;
  size_t size;
  mstrset_bucket_t* buckets;
  mstrset_item_t* strs;
} mstrset_t;


void mstrset_vec_init(mstrset_vec_t* vec);
void mstrset_vec_push(mstrset_vec_t* vec, char *str, size_t len, mstrset_hash_t hash);
void mstrset_vec_destroy(mstrset_vec_t* vec);

mstrset_t* mstrset_new();
int mstrset_bucket_contains(mstrset_bucket_t* bucket, char* str, size_t len, mstrset_hash_t hash);
int mstrset_contains(mstrset_t* set, mstrset_item_t str);
void mstrset_insert(mstrset_t* set, char* str,  size_t len);
void mstrset_destroy(mstrset_t* set);

#endif
