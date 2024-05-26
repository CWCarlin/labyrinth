#pragma once

#include "allocators/alloc_types.h"
#include "data_structures/hash.h"
#include "utils/types.h"

typedef struct lbr_set_t {
  LbrAllocCallbacks alloc_callbacks;
  u8* data;
  usize capacity;
  usize key_size;
  usize length;
  PFN_lbrHashFunc hash_func;
  PFN_lbrEqualityFunc equality_func;
} LbrSet;

typedef struct lbr_set_create_info_t {
  usize capacity;
  usize key_size;
  usize length;
  LbrAllocCallbacks alloc_callbacks;
  PFN_lbrHashFunc hash_func;
  PFN_lbrEqualityFunc equality_func;
} LbrSetCreateInfo;

void lbrDefineSet(LbrSetCreateInfo* p_info, LbrSet* p_set);
void lbrDestroySet(LbrSet* p_set);
void lbrSetInsert(LbrSet* p_set, void* p_key);
void lbrSetRemove(LbrSet* p_set, void* p_key);
u8 lbrSetContains(LbrSet* p_set, void* p_key);
void lbrSetClear(LbrSet* p_set);
