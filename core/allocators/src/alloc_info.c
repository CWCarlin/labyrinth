#include "allocators/alloc_info.h"

void* lbrAllocInfoAllocate(LbrAllocInfo* p_alloc_info) {
	return p_alloc_info->allocate(p_alloc_info->p_allocator, p_alloc_info->bytes);
}

void lbrAllocInfoFree(LbrAllocInfo* p_alloc_info, void* block) {
	p_alloc_info->free(p_alloc_info->p_allocator, block);
}
