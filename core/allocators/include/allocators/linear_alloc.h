#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_linear_allocator_t {
	u8* data;
	usize size;
	usize top;
} LbrLinearAllocator;

void lbrCreateLinearAllocator(LbrLinearAllocator* p_allocator, usize bytes);
void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator);
LbrAllocInfo lbrLinearAllocatorGetAllocInfo(LbrLinearAllocator* p_allocator, usize bytes);
void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes);
void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator);
