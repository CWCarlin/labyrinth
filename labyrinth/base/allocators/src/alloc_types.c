#include "allocators/alloc_types.h"

void* lbrAllocCallbacksAllocate(LbrAllocCallbacks* p_callbacks, usize capacity) {
  return p_callbacks->pfn_alloc(p_callbacks->p_allocator, capacity);
}

void* lbrAllocCallbacksReallocate(LbrAllocCallbacks* p_callbacks, void* block, usize capacity) {
  return p_callbacks->pfn_realloc(p_callbacks->p_allocator, block, capacity);
}

void lbrAllocCallbacksFree(LbrAllocCallbacks* p_callbacks, void* block) {
  p_callbacks->pfn_free(p_callbacks->p_allocator, block);
}
