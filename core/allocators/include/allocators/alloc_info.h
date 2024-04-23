#pragma once

#include "utility/types.h"

typedef void* (*LbrAllocatorAllocFunc)(void* p_allocator, usize bytes);
typedef void* (*LbrAllocatorFreeFunc)(void* p_allocator, void* block);

typedef struct lbr_alloc_info_t {
	void* p_allocator;
	LbrAllocatorAllocFunc allocate;
	LbrAllocatorFreeFunc free;
	usize bytes;
} LbrAllocInfo;

void* lbrAllocInfoAllocate(LbrAllocInfo* p_alloc_info);
void lbrAllocInfoFree(LbrAllocInfo* p_alloc_info, void* block);
