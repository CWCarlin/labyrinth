#include "allocators/linear_alloc.h"

#include <stdlib.h> // IWYU pragma: keep

#include "utility/logging.h"
#include "utility/types.h"

void lbrCreateLinearAllocator(LbrLinearAllocator* p_allocator, usize bytes) {
	p_allocator->data = malloc(bytes);
	p_allocator->size = bytes;
	p_allocator->top = 0;
}

void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator) {
	free(p_allocator->data);
	p_allocator->size = 0;
	p_allocator->top = 0;
}

void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes) {
	if (p_allocator->top + bytes >= p_allocator->size) {
		LOG_ERROR("attempting to allocate more bytes than available from a linear allocator");
	}
	p_allocator->top += bytes;
	return (void*)(p_allocator->data + p_allocator->top - bytes);
}

void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator) {
	p_allocator->top = 0;
}
