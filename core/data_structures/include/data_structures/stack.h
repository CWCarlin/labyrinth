#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_stack_t {
	LbrAllocCallback alloc_callback;
	u8* data;
	usize capacity;
	usize type_size;
	
	usize top;
} LbrStack;

void lbrCreateStack(LbrStack* p_stack, usize capacity, usize type_size, LbrAllocCallback alloc_callback);
void lbrDestroyStack(LbrStack* p_stack);
void lbrStackPush(LbrStack* p_stack, void* p_data_in);
void lbrStackPop(LbrStack* p_stack, void* p_data_out);
void lbrStackClear(LbrStack* p_stack);
