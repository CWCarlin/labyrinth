#pragma once

// this header is cursed

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wnullability-extension"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "renderer/core.h"
#include "utils/types.h"

typedef struct lbr_gpu_allocator_t {
  VmaAllocator allocator;
} LbrGPUAllocator;

LbrGPUAllocator lbrCreateGPUAllocator(LbrVulkanCore* p_core, LbrDevice* p_device);
void lbrDestroyGPUAllocator(LbrGPUAllocator* p_allocator);

typedef struct lbr_allocated_buffer_t {
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo info;
} LbrAllocatedBuffer;

LbrAllocatedBuffer lbrCreateAllocatedBuffer(LbrGPUAllocator* p_allocator, usize allocation_size, VkBufferUsageFlags usage);
void lbrDestroyAllocatedBuffer(LbrAllocatedBuffer* p_buffer, LbrGPUAllocator* p_allocator);
