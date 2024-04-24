#include "allocators/linear_alloc.h"

#include <stdlib.h> // IWYU pragma: keep

#include "allocators/alloc_info.h"
#include "utility/logging.h"
#include "utility/types.h"

void lbrCreateLinearAllocator(LbrLinearAllocator* p_allocator, usize bytes) {
	p_allocator->data = malloc(bytes);
	p_allocator->capacity = bytes;
	p_allocator->top = 0;
}

void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator) {
	free(p_allocator->data);
	p_allocator->capacity = 0;
	p_allocator->top = 0;
}

LbrAllocCallback lbrLinearAllocatorGetAllocCallback(LbrLinearAllocator* p_allocator) {
	LbrAllocCallback alloc_callback;
	alloc_callback.p_allocator = p_allocator;
	alloc_callback.pfn_allocate = (PFN_lbrAllocationFunc)&lbrLinearAllocatorAllocate;
	alloc_callback.pfn_free = NULL;

	return alloc_callback;
}

void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes) {
	if (p_allocator->top + bytes > p_allocator->capacity) {
		LOG_ERROR("attempting to allocate more bytes than available from a linear allocator");
	}
	p_allocator->top += bytes;
	return (void*)(p_allocator->data + p_allocator->top - bytes);
}

void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator) {
	p_allocator->top = 0;
}
