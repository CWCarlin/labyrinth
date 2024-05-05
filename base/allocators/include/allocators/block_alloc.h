#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

typedef struct lbr_block_allocator_t {
  u8* data;
  usize bytes;
  usize alignment;
  usize block_size;
  u8** next;
  LbrAllocCallbacks lbr_callbacks;
  VkAllocationCallbacks vk_callbacks;
} LbrBlockAllocator;

typedef struct lbr_block_allocator_create_info_t {
  usize num_blocks;
  usize block_size;
  usize alignment;
} LbrBlockAllocatorCreateInfo;

void lbrCreateBlockAllocator(LbrBlockAllocatorCreateInfo* p_info, LbrBlockAllocator* p_allocator);
void lbrDestroyBlockAllocator(LbrBlockAllocator* p_allocator);
void* lbrBlockAllocatorAllocate(LbrBlockAllocator* p_allocator);
void lbrBlockAllocatorFree(LbrBlockAllocator* p_allocator, void* block);
void lbrBlockAllocatorClear(LbrBlockAllocator* p_allocator);

void* lbrBlockAllocatorVkAllocate(void* p_allocator, usize bytes, usize alignment, VkSystemAllocationScope scope);
void* lbrBlockAllocatorVkReallocate(void* p_allocator, void* block, usize bytes, usize alignment, VkSystemAllocationScope scope);
void lbrBlockAllocatorVkFree(void* p_allocator, void* block);
