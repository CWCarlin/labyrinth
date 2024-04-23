#pragma once

#include "allocators/alloc_info.h"
#include "utility/types.h"

typedef struct lbr_stack_t {
	LbrAllocInfo alloc_info;
	u8* data;
	usize type_size;
	usize length;
	usize top;
} LbrStack;

void lbrCreateStack(LbrStack* p_stack, usize type_size, LbrAllocInfo alloc_info);
void lbrDestroyStack(LbrStack* p_stack);
void lbrStackPush(LbrStack* p_stack, void* data);
void* lbrStackPop(LbrStack* p_stack);
void lbrStackClear(LbrStack* p_stack);
