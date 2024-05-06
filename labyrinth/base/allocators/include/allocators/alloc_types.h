#pragma once

#include "utils/types.h"

typedef void* (*PFN_lbrAllocFunction)(void* p_allocator, usize bytes);
typedef void* (*PFN_lbrReallocFunction)(void* p_allocator, void* block, usize bytes);
typedef void (*PFN_lbrFreeFunction)(void* p_allocator, void* block);

typedef struct lbr_alloc_callbacks_t {
  void* p_allocator;
  PFN_lbrAllocFunction pfn_alloc;
  PFN_lbrReallocFunction pfn_realloc;
  PFN_lbrFreeFunction pfn_free;
} LbrAllocCallbacks;

void* lbrAllocCallbacksAllocate(LbrAllocCallbacks* p_callbacks, usize capacity);
void* lbrAllocCallbacksReallocate(LbrAllocCallbacks* p_callbacks, void* block, usize capacity);
void lbrAllocCallbacksFree(LbrAllocCallbacks* p_callbacks, void* block);
