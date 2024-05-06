#include "data_structures/set.h"

#include <string.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"

static usize lbrSetFind(LbrSet* p_set, void* p_key) {
  usize idx      = p_set->hash_func(p_key) % p_set->capacity;
  usize base_idx = idx;
  while (!p_set->equality_func(p_key, p_set->data + (idx * p_set->key_size))) {
    u8 open_block = 1;
    for (usize i = 0; i < p_set->key_size; i++) {
      if (*(p_set->data + (idx * p_set->key_size) + i) != 0) {
        open_block = 0;
        break;
      }
    }

    if (open_block) {
      break;
    }

    idx = (idx + 1) % p_set->capacity;
    if (idx == base_idx) {
      break;
    }
  }

  return idx;
}

void lbrCreateSet(LbrSetCreateInfo* p_info, LbrSet* p_set) {
  usize capacity = p_info->capacity;
  usize key_size = p_info->key_size;

  p_set->alloc_callbacks = p_info->alloc_callbacks;
  p_set->data            = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, capacity * key_size);
  p_set->capacity        = p_info->capacity;
  p_set->key_size        = key_size;
  p_set->hash_func       = p_info->hash_func;
  p_set->equality_func   = p_info->equality_func;
  lbrSetClear(p_set);
}

void lbrDestroySet(LbrSet* p_set) {
  lbrAllocCallbacksFree(&p_set->alloc_callbacks, p_set->data);
  p_set->capacity = 0;
  p_set->key_size = 0;
  p_set->length   = 0;
}

void lbrSetInsert(LbrSet* p_set, void* p_key) {
  LBR_ASSERT(p_set->length != p_set->capacity);
  usize idx = lbrSetFind(p_set, p_key);
  memcpy(p_set->data + (idx * p_set->key_size), p_key, p_set->key_size);
  p_set->length++;
}

void lbrSetRemove(LbrSet* p_set, void* p_key) {
  usize idx = lbrSetFind(p_set, p_key);
  if (p_set->equality_func(p_key, p_set->data + (idx * p_set->key_size))) {
    memset(p_set->data + (idx * p_set->key_size), 0, p_set->key_size);
    p_set->length--;
  }
}

u8 lbrSetContains(LbrSet* p_set, void* p_key) {
  usize idx = lbrSetFind(p_set, p_key);
  if (p_set->equality_func(p_key, p_set->data + (idx * p_set->key_size))) {
    return 1;
  }

  return 0;
}

void lbrSetClear(LbrSet* p_set) {
  memset(p_set->data, 0, p_set->capacity * p_set->key_size);
  p_set->length = 0;
}
