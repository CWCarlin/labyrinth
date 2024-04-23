#include "allocators/block_alloc.h"

#include <stdlib.h> // IWYU pragma: keep
#include <stdio.h>
#include <string.h>

#include "utility/logging.h"
#include "utility/types.h"

typedef struct lbr_data_block_t {
	struct lbr_data_block_t* next;
} LbrDataBlock;

void lbrCreateBlockAllocator(LbrBlockAllocator* p_allocator, usize num_blocks, usize block_size) {
	if (block_size < 8) {
		block_size = 8;
	}
	p_allocator->data = calloc(num_blocks, block_size);
	p_allocator->next = p_allocator->data;
	p_allocator->block_size = block_size;
	p_allocator->size = num_blocks * block_size;
}

void lbrDestroyBlockAllocator(LbrBlockAllocator* p_allocator) {
	free(p_allocator->data);
}

LbrAllocInfo lbrBlockAllocatorGetAllocInfo(LbrBlockAllocator* p_allocator, usize bytes) {
	if (p_allocator->block_size < bytes) {
		LOG_ERROR("attempting to create alloc info with size larger than block allocator block size");
	}

	LbrAllocInfo alloc_info;
	alloc_info.p_allocator = p_allocator;
	alloc_info.allocate = (LbrAllocatorAllocFunc)&lbrBlockAllocatorAllocate;
	alloc_info.free = (LbrAllocatorFreeFunc)&lbrBlockAllocatorFree;
	alloc_info.bytes = bytes;

	return alloc_info;
}

void* lbrBlockAllocatorAllocate(LbrBlockAllocator* p_allocator) {
	if (p_allocator->next >= p_allocator->data + p_allocator->size || p_allocator->next < p_allocator->data) {
		LOG_ERROR("attempting to allocate a block from a full block allocator");
	}
	LbrDataBlock* block = (void*)p_allocator->next;
	p_allocator->next = (void*)block->next;
	if (p_allocator->next == NULL) {
		p_allocator->next = (u8*)block + p_allocator->block_size;
	}

	return (void*)block;
}

void lbrBlockAllocatorFree(LbrBlockAllocator* p_allocator, void* block) {
	if ((u8*)block >= p_allocator->data + p_allocator->size || (u8*)block < p_allocator->data) {
		LOG_ERROR("attempting to free block not allocated from block allocator");
	}
	((LbrDataBlock*)block)->next = (LbrDataBlock*)p_allocator->next;
	p_allocator->next = block;
}

void lbrBlockAllocatorClear(LbrBlockAllocator* p_allocator) {
	memset(p_allocator->data, 0, p_allocator->size);
	p_allocator->next = p_allocator->data;
}
