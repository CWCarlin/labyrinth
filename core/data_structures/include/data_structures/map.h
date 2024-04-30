#pragma once

#include "allocators/alloc_info.h"
#include "data_structures/hash.h"

typedef struct lbr_map_t {
	LbrAllocCallback alloc_callback;
	u8* data;
	usize capacity;
	usize key_size;
	usize value_size;
	usize length;

	PFN_lbrHashFunc hash;
	PFN_lbrEqualityFunc equals;
} LbrMap;

void lbrCreateMap(LbrMap* p_map, usize capacity, usize key_size, usize value_size,
	LbrAllocCallback alloc_callback, PFN_lbrHashFunc hash, PFN_lbrEqualityFunc equals);
void lbrDestroyMap(LbrMap* p_map);
void lbrMapInsert(LbrMap* p_map, void* p_key, void* p_value);
void* lbrMapGetValue(LbrMap* p_map, void* p_key);
void lbrMapRemove(LbrMap* p_map, void* p_key);
u8 lbrMapContainsKey(LbrMap* p_map, void* p_key);
void lbrMapClear(LbrMap* p_map);
