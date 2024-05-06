#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

typedef struct lbr_linear_allocator_t {
  u8* data;
  usize bytes;
  usize alignment;
  usize top;
  LbrAllocCallbacks lbr_callbacks;
  VkAllocationCallbacks vk_callbacks;
} LbrLinearAllocator;

typedef struct lbr_linear_allocator_create_info_t {
  usize bytes;
  usize alignment;
} LbrLinearAllocatorCreateInfo;

void lbrCreateLinearAllocator(LbrLinearAllocatorCreateInfo* p_info, LbrLinearAllocator* p_allocator);
void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator);
void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes);
void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator);

void* lbrLinearAllocatorVkAllocate(void* p_allocator, usize bytes, usize alignment, VkSystemAllocationScope scope);
void* lbrLinearAllocatorVkReallocate(void* p_allocator, void* block, usize bytes, usize alignment, VkSystemAllocationScope scope);
void lbrLinearAllocatorVkFree(void* p_allocator, void* block);
