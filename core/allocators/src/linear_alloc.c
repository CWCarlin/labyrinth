#include "allocators/linear_alloc.h"

#include <stdlib.h> // IWYU pragma: keep

#include "utility/logging.h"
#include "utility/types.h"

void lbrCreateLinearAllocator(LbrLinearAllocator* p_allocator, usize bytes) {
	p_allocator->data = malloc(bytes);
	p_allocator->length = bytes;
	p_allocator->top = 0;
}

void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator) {
	free(p_allocator->data);
	p_allocator->length = 0;
	p_allocator->top = 0;
}

LbrAllocInfo lbrLinearAllocatorGetAllocInfo(LbrLinearAllocator* p_allocator, usize bytes) {
	LbrAllocInfo alloc_info;
	alloc_info.p_allocator = p_allocator;
	alloc_info.allocate = (LbrAllocatorAllocFunc)&lbrLinearAllocatorAllocate;
	alloc_info.free = NULL;
	alloc_info.bytes = bytes;

	return alloc_info;
}

void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes) {
	if (p_allocator->top + bytes > p_allocator->length) {
		LOG_ERROR("attempting to allocate more bytes than available from a linear allocator");
	}
	p_allocator->top += bytes;
	return (void*)(p_allocator->data + p_allocator->top - bytes);
}

void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator) {
	p_allocator->top = 0;
}
