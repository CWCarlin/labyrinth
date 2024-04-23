#include "data_structures/stack.h"
#include "utility/logging.h"

void lbrCreateStack(LbrStack* p_stack, LbrAllocInfo alloc_info) {
	p_stack->alloc_info = alloc_info;
	p_stack->data = lbrAllocInfoAllocate(&alloc_info);
	p_stack->size = alloc_info.bytes;
	p_stack->top = 0;
}

void lbrDestroyStack(LbrStack* p_stack) {
	lbrAllocInfoFree(&p_stack->alloc_info, p_stack->data);
	p_stack->size = 0;
	p_stack->top = 0;
}

void* lbrStackAllocate(LbrStack* p_stack, usize bytes) {
	if (p_stack->top + bytes > p_stack->size) {
		LOG_ERROR("attempting to allocate memory from a fully allocated stack");
	}
	p_stack->top += bytes;
	return p_stack->data + p_stack->top - bytes;
}

void lbrStackClear(LbrStack* p_stack) {
	p_stack->top = 0;
}
