#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_stack_t {
	LbrAllocInfo alloc_info;
	u8* data;
	usize size;
	usize top;
} LbrStack;

void lbrCreateStack(LbrStack* p_stack, LbrAllocInfo alloc_info);
void lbrDestroyStack(LbrStack* p_stack);
void* lbrStackAllocate(LbrStack* p_stack, usize bytes);
void lbrStackClear(LbrStack* p_stack);
