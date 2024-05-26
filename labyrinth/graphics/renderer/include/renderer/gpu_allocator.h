#include "renderer/core.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wnullability-extension"
#include "vma/vk_mem_alloc.h"
#pragma clang diagnostic pop

typedef struct lbr_device_allocator_t {
  VmaAllocator allocator;
  VkDescriptorPool descriptor_pool;
  VkDescriptorType type;
  double ratio;
} LbrDeviceAllocator;

typedef struct lbr_allocated_buffer {
  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo info;
} LbrAllocatedBuffer;

LbrDeviceAllocator lbrCreateDeviceAllocator(LbrVulkanCore* p_core, LbrLogicalDevice* p_device);
