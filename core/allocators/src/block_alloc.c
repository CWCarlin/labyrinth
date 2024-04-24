#include "allocators/block_alloc.h"

#include <stdlib.h> // IWYU pragma: keep
#include <stdio.h>
#include <string.h>

#include "allocators/alloc_info.h"
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
	p_allocator->capacity = num_blocks * block_size;
}

void lbrDestroyBlockAllocator(LbrBlockAllocator* p_allocator) {
	free(p_allocator->data);
	p_allocator->capacity = 0;
	p_allocator->block_size = 0;
	p_allocator->next = NULL;
}

LbrAllocCallback lbrBlockAllocatorGetAllocInfo(LbrBlockAllocator* p_allocator) {
	LbrAllocCallback alloc_callback;
	alloc_callback.p_allocator = p_allocator;
	alloc_callback.pfn_allocate = (PFN_lbrAllocationFunc)&lbrBlockAllocatorAllocate;
	alloc_callback.pfn_free = (PFN_lbrFreeFunc)&lbrBlockAllocatorFree;

	return alloc_callback;
}

void* lbrBlockAllocatorAllocate(LbrBlockAllocator* p_allocator) {
	if (p_allocator->next >= p_allocator->data + p_allocator->capacity || p_allocator->next < p_allocator->data) {
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
	if ((u8*)block >= p_allocator->data + p_allocator->capacity || (u8*)block < p_allocator->data) {
		LOG_ERROR("attempting to free block not allocated from block allocator");
	}
	((LbrDataBlock*)block)->next = (LbrDataBlock*)p_allocator->next;
	p_allocator->next = block;
}

void lbrBlockAllocatorClear(LbrBlockAllocator* p_allocator) {
	memset(p_allocator->data, 0, p_allocator->capacity);
	p_allocator->next = p_allocator->data;
}
