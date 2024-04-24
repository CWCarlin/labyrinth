#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_block_allocator_t {
	u8* data;
	u8* next;
	usize block_size;
	usize capacity;
} LbrBlockAllocator;

void lbrCreateBlockAllocator(LbrBlockAllocator* p_allocator, usize num_blocks, usize block_size);
void lbrDestroyBlockAllocator(LbrBlockAllocator* p_allocator);
LbrAllocCallback lbrBlockAllocatorGetAllocCallback(LbrBlockAllocator* p_allocator);
void* lbrBlockAllocatorAllocate(LbrBlockAllocator* p_allocator);
void lbrBlockAllocatorFree(LbrBlockAllocator* p_allocator, void* block);
void lbrBlockAllocatorClear(LbrBlockAllocator* p_allocator);
