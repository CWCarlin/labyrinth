#pragma once

#include "utility/types.h"

typedef void* (*PFN_lbrAllocationFunc)(void* p_allocator, usize bytes);
typedef void* (*PFN_lbrFreeFunc)(void* p_allocator, void* block);

typedef struct lbr_alloc_callback_t {
	void* p_allocator;
	PFN_lbrAllocationFunc pfn_allocate;
	PFN_lbrFreeFunc pfn_free;
} LbrAllocCallback;

void* lbrAllocCallbackAllocate(LbrAllocCallback* p_alloc_callback, usize bytes);
void lbrAllocCallbackFree(LbrAllocCallback* p_alloc_callback, void* block);
