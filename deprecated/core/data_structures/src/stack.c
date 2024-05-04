#include "data_structures/stack.h"

#include <string.h> // IWYU pragma: keep

#include "allocators/alloc_info.h"
#include "utility/logging.h"

void lbrCreateStack(LbrStack* p_stack, usize capacity, usize type_size, LbrAllocCallback alloc_callback) {
	p_stack->alloc_callback = alloc_callback;
	p_stack->data = lbrAllocCallbackAllocate(&alloc_callback, capacity * type_size);
	p_stack->capacity = capacity;
	p_stack->type_size = type_size;
	lbrStackClear(p_stack);
}

void lbrDestroyStack(LbrStack* p_stack) {
	lbrAllocCallbackFree(&p_stack->alloc_callback, p_stack->data);
	p_stack->capacity = 0;
	p_stack->type_size = 0;
	p_stack->length = 0;
}

void lbrStackPush(LbrStack* p_stack, void* p_data_in) {
	if (p_stack->capacity == p_stack->length) {
		LOG_ERROR("attempting to push data to a full stack");
	}
	memcpy(p_stack->data + (p_stack->length * p_stack->type_size), p_data_in, p_stack->type_size);
	p_stack->length++;
}

void lbrStackPop(LbrStack* p_stack, void* data) {
	if (p_stack->length == 0) {
		LOG_ERROR("attempting to pop data from an empty stack");
	}
	p_stack--;
	memcpy(data, p_stack->data + (p_stack->length * p_stack->type_size), p_stack->type_size);
}

void lbrStackClear(LbrStack* p_stack) {
	p_stack->length = 0;
}
