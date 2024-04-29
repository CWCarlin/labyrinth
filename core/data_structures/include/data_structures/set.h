#pragma once

#include "allocators/alloc_info.h"
#include "data_structures/hash.h"
#include "utility/types.h"

typedef struct lbr_set_t {
	LbrAllocCallback alloc_callback;
	u8* data;
	usize capacity;
	usize type_size;
	usize length;

	PFN_lbrHashFunc hash;
	PFN_lbrEqualityFunc equals;
} LbrSet;

void lbrCreateSet(LbrSet* p_set, usize capacity, usize type_size,
	LbrAllocCallback alloc_callback, PFN_lbrHashFunc hash, PFN_lbrEqualityFunc equals);
void lbrDestroySet(LbrSet* p_set);
void lbrSetInsert(LbrSet* p_set, void* p_data_in);
void lbrSetRemove(LbrSet* p_set, void* p_data);
u8 lbrSetContains(LbrSet* p_set, void* p_data);
void lbrSetClear(LbrSet* p_set);
