#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_linear_allocator_t {
	u8* data;
	usize bytes;
	usize top;
} LbrLinearAllocator;

void lbrCreateLinearAllocator(LbrLinearAllocator* p_allocator, usize bytes);
void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator);
LbrAllocCallback lbrLinearAllocatorGetAllocCallback(LbrLinearAllocator* p_allocator);
void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes);
void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator);
