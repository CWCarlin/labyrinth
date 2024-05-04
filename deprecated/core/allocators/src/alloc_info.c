#include "allocators/alloc_info.h"

void* lbrAllocCallbackAllocate(LbrAllocCallback* p_alloc_callback, usize bytes) {
	return p_alloc_callback->pfn_allocate(p_alloc_callback->p_allocator, bytes);
}

void lbrAllocCallbackFree(LbrAllocCallback* p_alloc_callback, void* block) {
	p_alloc_callback->pfn_free(p_alloc_callback->p_allocator, block);
}
