#include "data_structures/stack.h"

#include "utils/logging.h"
#include "utils/types.h"

#include <stdlib.h> // IWYU pragma: keep

void lbrCreateDataStack(usize size, LbrDataStack* stack) {
	stack->data = malloc(sizeof(void*) * size);
	stack->top = 0xFFFFFFFFFFFFFFFF;
	stack->size = size;
}

void lbrDestroyDataStack(LbrDataStack* stack) {
	for (usize i = stack->top; i >= 0; i--) {
		free(stack->data[i]);
	}
	stack->top = 0;
	stack->size = 0;
	free(stack->data);
}

void lbrPushDataStack(LbrDataStack* stack, void* data) {
	if (stack->top == stack->size) {
		LOG_ERROR("pushing to a full datastack");
	}
	stack->data[++stack->top] = data;
}

void* lbrPopDataStack(LbrDataStack* stack) {
	if (lbrIsDataStackEmpty(stack)) {
		LOG_ERROR("popping from an empty datastack");
	}
	return stack->data[stack->top--];
}

LbrBool lbrIsDataStackEmpty(const LbrDataStack* stack) {
	if (stack->top == 0xFFFFFFFFFFFFFFFF) return LBR_TRUE;
	return LBR_FALSE;
}
