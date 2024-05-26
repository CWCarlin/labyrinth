#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"

typedef struct lbr_stack_t {
  LbrAllocCallbacks alloc_callbacks;
  u8* data;
  usize capacity;
  usize type_size;
  usize length;
} LbrStack;

typedef struct lbr_stack_create_info_t {
  usize capacity;
  usize type_size;
  LbrAllocCallbacks alloc_callbacks;
} LbrStackCreateInfo;

void lbrDefineStack(LbrStackCreateInfo* p_info, LbrStack* p_stack);
void lbrDestroyStack(LbrStack* p_stack);
void lbrStackPush(LbrStack* p_stack, void* p_data_in);
void lbrStackPop(LbrStack* p_stack, void* p_data_out);
void lbrStackClear(LbrStack* p_stack);
