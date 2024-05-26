#pragma once

#include "allocators/alloc_types.h"
#include "data_structures/hash.h"
#include "utils/types.h"

typedef struct lbr_map_t {
  LbrAllocCallbacks alloc_callbacks;
  u8* data;
  usize capacity;
  usize key_size;
  usize value_size;
  usize length;
  PFN_lbrHashFunc hash_func;
  PFN_lbrEqualityFunc equality_func;
} LbrMap;

typedef struct lbr_map_create_info_t {
  usize capacity;
  usize key_size;
  usize value_size;
  LbrAllocCallbacks alloc_callbacks;
  PFN_lbrHashFunc hash_func;
  PFN_lbrEqualityFunc equality_func;
} LbrMapCreateInfo;

void lbrDefineMap(LbrMapCreateInfo* p_info, LbrMap* p_map);
void lbrDestroyMap(LbrMap* p_map);
void lbrMapInsert(LbrMap* p_map, void* p_key, void* p_value);
void* lbrMapGetValue(const LbrMap* p_map, void* p_key);
u8 lbrMapContainsKey(const LbrMap* p_map, void* p_key);
void lbrMapClear(LbrMap* p_map);
