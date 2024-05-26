#include "data_structures/stack.h"

#include <string.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"

void lbrDefineStack(LbrStackCreateInfo* p_info, LbrStack* p_stack) {
  usize capacity  = p_info->capacity;
  usize type_size = p_info->type_size;

  p_stack->alloc_callbacks = p_info->alloc_callbacks;
  p_stack->data            = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, capacity * type_size);
  p_stack->capacity        = capacity;
  p_stack->type_size       = type_size;
  lbrStackClear(p_stack);
}

void lbrDestroyStack(LbrStack* p_stack) {
  lbrAllocCallbacksFree(&p_stack->alloc_callbacks, p_stack->data);
  p_stack->capacity  = 0;
  p_stack->type_size = 0;
  p_stack->length    = 0;
}

void lbrStackPush(LbrStack* p_stack, void* p_data_in) {
  LBR_ASSERT(p_stack->capacity != p_stack->length);
  memcpy(p_stack->data + (p_stack->length * p_stack->type_size), p_data_in, p_stack->type_size);
  p_stack->length++;
}

void lbrStackPop(LbrStack* p_stack, void* p_data_out) {
  LBR_ASSERT(p_stack->length != 0);
  p_stack->length--;
  memcpy(p_data_out, p_stack->data + (p_stack->length * p_stack->type_size), p_stack->type_size);
}

void lbrStackClear(LbrStack* p_stack) { p_stack->length = 0; }
