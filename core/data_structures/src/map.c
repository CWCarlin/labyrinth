#include "data_structures/map.h"

#include <string.h>

#include "allocators/alloc_info.h"
#include "utility/logging.h"

void lbrCreateMap(LbrMap* p_map, usize capacity, usize key_size,
                  usize value_size, LbrAllocCallback alloc_callback,
                  PFN_lbrHashFunc hash, PFN_lbrEqualityFunc equals) {
  p_map->alloc_callback = alloc_callback;
  p_map->data = lbrAllocCallbackAllocate(&alloc_callback,
                                         capacity * (key_size + value_size));
  p_map->capacity = capacity;
  p_map->key_size = key_size;
  p_map->value_size = value_size;
  p_map->hash = hash;
  p_map->equals = equals;
  lbrMapClear(p_map);
}

void lbrDestroyMap(LbrMap* p_map) {
  lbrAllocCallbackFree(&p_map->alloc_callback, p_map->data);
  p_map->capacity = 0;
  p_map->key_size = 0;
  p_map->value_size = 0;
  p_map->length = 0;
}

void lbrMapInsert(LbrMap* p_map, void* p_key, void* p_value) {
  if (p_map->length == p_map->capacity) {
    LOG_ERROR("attempting to insert into a full map");
  }
  usize idx = p_map->hash(p_key) % p_map->capacity;
  while (!p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
    u8 open_block = 1;
    for (usize i = 0; i < p_map->key_size; i++) {
      if (*(p_map->data + (idx * p_map->key_size) + i) != 0) {
        open_block = 0;
        break;
      }
    }

    if (open_block) {
      memcpy(p_map->data + (idx * p_map->key_size), p_key, p_map->key_size);
      memcpy(p_map->data + (p_map->capacity * p_map->key_size) +
                 (idx * p_map->value_size),
             p_value, p_map->value_size);
      p_map->length++;
      break;
    }

    idx = (idx + 1) % p_map->capacity;
  }
}

void* lbrMapGetValue(LbrMap* p_map, void* p_key) {
  usize idx = p_map->hash(p_key) % p_map->capacity;
  while (!p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
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
  }

  if (p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
    return p_map->data + (p_map->capacity * p_map->key_size) +
           (idx * p_map->value_size);
  }

  return 0;
}

void lbrMapRemove(LbrMap* p_map, void* p_key) {
  usize idx = p_map->hash(p_key) % p_map->capacity;
  while (!p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
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
  }

  if (p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
    memcpy(p_map->data + (idx * p_map->key_size), 0, p_map->key_size);
    memcpy(p_map->data + (p_map->capacity * p_map->key_size) +
               (idx * p_map->value_size),
           0, p_map->value_size);
    p_map->length--;
  }
}

u8 lbrMapContainsKey(LbrMap* p_map, void* p_key) {
  usize idx = p_map->hash(p_key) % p_map->capacity;
  while (!p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
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
  }

  if (p_map->equals(p_key, p_map->data + (idx * p_map->key_size))) {
    return 1;
  }

  return 0;
}

void lbrMapClear(LbrMap* p_map) {
  memset(p_map->data, 0,
         p_map->capacity * (p_map->key_size + p_map->value_size));
  p_map->length = 0;
}
