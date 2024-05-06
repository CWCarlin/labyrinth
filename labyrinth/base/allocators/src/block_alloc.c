#include "allocators/block_alloc.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

void lbrCreateBlockAllocator(LbrBlockAllocatorCreateInfo* p_info, LbrBlockAllocator* p_allocator) {
  usize num_blocks = p_info->num_blocks;
  usize alignment  = p_info->alignment;
  usize block_size = p_info->block_size;

  LBR_ALIGN(block_size, alignment);
  p_allocator->data       = calloc(num_blocks, block_size);
  p_allocator->bytes      = num_blocks * block_size;
  p_allocator->next       = (u8**)p_allocator->data;
  p_allocator->alignment  = alignment;
  p_allocator->block_size = block_size;

  p_allocator->lbr_callbacks.p_allocator = p_allocator;
  p_allocator->lbr_callbacks.pfn_alloc   = (PFN_lbrAllocFunction)lbrBlockAllocatorAllocate;
  p_allocator->lbr_callbacks.pfn_realloc = (PFN_lbrReallocFunction)LbrNOP;
  p_allocator->lbr_callbacks.pfn_free    = (PFN_lbrFreeFunction)&lbrBlockAllocatorFree;

  p_allocator->vk_callbacks.pUserData             = p_allocator;
  p_allocator->vk_callbacks.pfnAllocation         = (PFN_vkAllocationFunction)lbrBlockAllocatorVkAllocate;
  p_allocator->vk_callbacks.pfnReallocation       = (PFN_vkReallocationFunction)lbrBlockAllocatorVkReallocate;
  p_allocator->vk_callbacks.pfnFree               = (PFN_vkFreeFunction)&lbrBlockAllocatorVkFree;
  p_allocator->vk_callbacks.pfnInternalAllocation = NULL;
  p_allocator->vk_callbacks.pfnInternalFree       = NULL;
}

void lbrDestroyBlockAllocator(LbrBlockAllocator* p_allocator) {
  free(p_allocator->data);
  p_allocator->bytes      = 0;
  p_allocator->alignment  = 0;
  p_allocator->block_size = 0;
  p_allocator->next       = NULL;
}

void* lbrBlockAllocatorAllocate(LbrBlockAllocator* p_allocator) {
  LBR_ASSERT((u8*)p_allocator->next < p_allocator->data + p_allocator->bytes);
  LBR_ASSERT(p_allocator->next >= (u8**)p_allocator->data);

  u8* block         = (u8*)p_allocator->next;
  p_allocator->next = (u8**)*p_allocator->next;
  if (!p_allocator->next) {
    p_allocator->next = (u8**)(block + p_allocator->block_size);
  }

  return block;
}

void lbrBlockAllocatorFree(LbrBlockAllocator* p_allocator, void* block) {
  LBR_ASSERT((u8*)block < p_allocator->data + p_allocator->bytes);
  LBR_ASSERT((u8*)block >= p_allocator->data);

  u8** n            = block;
  *n                = (u8*)p_allocator->next;
  p_allocator->next = n;
}

void lbrBlockAllocatorClear(LbrBlockAllocator* p_allocator) {
  memset(p_allocator->data, 0, p_allocator->bytes);
  p_allocator->next = (u8**)p_allocator->data;
}

void* lbrBlockAllocatorVkAllocate(void* p_allocator, usize bytes, usize alignment, VkSystemAllocationScope scope) {
  (void)bytes;
  (void)alignment;
  (void)scope;

  return lbrBlockAllocatorAllocate(p_allocator);
}

void* lbrBlockAllocatorVkReallocate(void* p_allocator, void* block, usize bytes, usize alignment, VkSystemAllocationScope scope) {
  (void)p_allocator;
  (void)bytes;
  (void)alignment;
  (void)scope;

  return block;
}

void lbrBlockAllocatorVkFree(void* p_allocator, void* block) { lbrBlockAllocatorFree(p_allocator, block); }
