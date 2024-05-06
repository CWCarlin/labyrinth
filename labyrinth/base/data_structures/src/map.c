#include "data_structures/map.h"

#include <string.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"

static usize lbrMapFind(LbrMap* p_map, void* p_key) {
  usize idx      = p_map->hash_func(p_key) % p_map->capacity;
  usize base_idx = idx;
  while (!p_map->equality_func(p_key, p_map->data + (idx * p_map->key_size))) {
    u8 open_block = 1;
    for (usize i = 0; i < p_map->key_size; i++) {
      if (*(p_map->data + (idx * p_map->key_size) + i) != 0) {
        open_block = 0;
        break;
      }
    }

    if (open_block) {
      break;
    }

    idx = (idx + 1) % p_map->capacity;
    if (idx == base_idx) {
      break;
    }
  }

  return idx;
}

void lbrCreateMap(LbrMapCreateInfo* p_info, LbrMap* p_map) {
  usize capacity   = p_info->capacity;
  usize key_size   = p_info->key_size;
  usize value_size = p_info->value_size;

  p_map->alloc_callbacks = p_info->alloc_callbacks;
  p_map->data            = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, capacity * (key_size + value_size));
  p_map->capacity        = capacity;
  p_map->key_size        = key_size;
  p_map->value_size      = value_size;
  p_map->hash_func       = p_info->hash_func;
  p_map->equality_func   = p_info->equality_func;
  lbrMapClear(p_map);
}

void lbrDestroyMap(LbrMap* p_map) {
  lbrAllocCallbacksFree(&p_map->alloc_callbacks, p_map->data);
  p_map->capacity   = 0;
  p_map->key_size   = 0;
  p_map->value_size = 0;
  p_map->length     = 0;
}

void lbrMapInsert(LbrMap* p_map, void* p_key, void* p_value) {
  LBR_ASSERT(p_map->length != p_map->capacity);
  usize idx = lbrMapFind(p_map, p_key);

  memcpy(p_map->data + (idx * p_map->key_size), p_key, p_map->key_size);
  memcpy(p_map->data + (p_map->capacity * p_map->key_size) + (idx * p_map->value_size), p_value, p_map->value_size);
  p_map->length++;
}

void* lbrMapGetValue(LbrMap* p_map, void* p_key) {
  usize idx = lbrMapFind(p_map, p_key);
  if (p_map->equality_func(p_key, p_map->data + (idx * p_map->key_size))) {
    return p_map->data + (p_map->capacity * p_map->key_size) + (idx * p_map->value_size);
  }

  return 0;
}

void lbrMapRemove(LbrMap* p_map, void* p_key) {
  usize idx = lbrMapFind(p_map, p_key);
  if (p_map->equality_func(p_key, p_map->data + (idx * p_map->key_size))) {
    memset(p_map->data + (idx * p_map->key_size), 0, p_map->key_size);
    memset(p_map->data + (p_map->capacity * p_map->key_size) + (idx * p_map->value_size), 0, p_map->value_size);
    p_map->length--;
  }
}

u8 lbrMapContainsKey(LbrMap* p_map, void* p_key) {
  usize idx = lbrMapFind(p_map, p_key);
  if (p_map->equality_func(p_key, p_map->data + (idx * p_map->key_size))) {
    return 1;
  }

  return 0;
}

void lbrMapClear(LbrMap* p_map) {
  memset(p_map->data, 0, p_map->capacity * (p_map->key_size + p_map->value_size));
  p_map->length = 0;
}
