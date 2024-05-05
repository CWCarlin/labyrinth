#include "allocators/linear_alloc.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "allocators/alloc_types.h"
#include "utils/logging.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

void lbrCreateLinearAllocator(LbrLinearAllocatorCreateInfo* p_info, LbrLinearAllocator* p_allocator) {
  usize bytes     = p_info->bytes;
  usize alignment = p_info->alignment;

  LBR_ALIGN(bytes, alignment);

  p_allocator->data      = malloc(bytes);
  p_allocator->bytes     = bytes;
  p_allocator->alignment = alignment;
  p_allocator->top       = 0;

  p_allocator->lbr_callbacks.p_allocator = p_allocator;
  p_allocator->lbr_callbacks.pfn_alloc   = (PFN_lbrAllocFunction)lbrLinearAllocatorAllocate;
  p_allocator->lbr_callbacks.pfn_realloc = (PFN_lbrReallocFunction)LbrNOP;
  p_allocator->lbr_callbacks.pfn_free    = (PFN_lbrFreeFunction)LbrNOP;

  p_allocator->vk_callbacks.pUserData             = p_allocator;
  p_allocator->vk_callbacks.pfnAllocation         = (PFN_vkAllocationFunction)lbrLinearAllocatorVkAllocate;
  p_allocator->vk_callbacks.pfnReallocation       = (PFN_vkReallocationFunction)lbrLinearAllocatorVkReallocate;
  p_allocator->vk_callbacks.pfnFree               = (PFN_vkFreeFunction)lbrLinearAllocatorVkFree;
  p_allocator->vk_callbacks.pfnInternalAllocation = NULL;
  p_allocator->vk_callbacks.pfnInternalFree       = NULL;
}

void lbrDestroyLinearAllocator(LbrLinearAllocator* p_allocator) {
  free(p_allocator->data);
  p_allocator->bytes     = 0;
  p_allocator->alignment = 0;
  p_allocator->top       = 0;
}

void* lbrLinearAllocatorAllocate(LbrLinearAllocator* p_allocator, usize bytes) {
  LBR_ALIGN(bytes, p_allocator->alignment);
  LBR_ASSERT((p_allocator->top + bytes <= p_allocator->bytes));
  p_allocator->top += bytes;

  return (void*)(p_allocator->data + p_allocator->top - bytes);
}

void lbrLinearAllocatorClear(LbrLinearAllocator* p_allocator) { p_allocator->top = 0; }

void* lbrLinearAllocatorVkAllocate(void* p_allocator, usize bytes, usize alignment, VkSystemAllocationScope scope) {
  (void)alignment;
  (void)scope;

  return lbrLinearAllocatorAllocate(p_allocator, bytes);
}

void* lbrLinearAllocatorVkReallocate(void* p_allocator, void* block, usize bytes, usize alignment,
                                     VkSystemAllocationScope scope) {
  (void)alignment;
  (void)scope;

  void* p = lbrLinearAllocatorAllocate(p_allocator, bytes);
  memcpy(p, block, bytes);

  return p;
}

void lbrLinearAllocatorVkFree(void* p_allocator, void* block) {
  (void)p_allocator;
  (void)block;
}
