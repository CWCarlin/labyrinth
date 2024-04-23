#include "data_structures/stack.h"
#include "allocators/alloc_info.h"

#include <string.h>

void lbrCreateStack(LbrStack* p_stack, usize type_size, LbrAllocInfo alloc_info) {
	p_stack->alloc_info = alloc_info;
	p_stack->data = lbrAllocInfoAllocate(&alloc_info);
	p_stack->type_size = type_size;
	p_stack->length = alloc_info.bytes;
	p_stack->top = 0;
}

void lbrDestroyStack(LbrStack* p_stack) {
	lbrAllocInfoFree(&p_stack->alloc_info, p_stack->data);
	p_stack->type_size = 0;
	p_stack->length = 0;
	p_stack->top = 0;
}

void lbrStackPush(LbrStack* p_stack, void* data) {
	memcpy(p_stack->data + p_stack->top, data, p_stack->type_size);
}

void* lbrStackPop(LbrStack* p_stack) {
	
}

void lbrStackClear(LbrStack* p_stack) {
	p_stack->top = 0;
}
