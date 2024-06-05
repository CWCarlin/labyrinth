#include "renderer/allocator.h"

#include <vulkan/vulkan_core.h>

#include "renderer/core.h"
#include "utils/logging.h"
#include "utils/types.h"

LbrGPUAllocator lbrCreateGPUAllocator(LbrVulkanCore* p_core, LbrDevice* p_device) {
  LbrGPUAllocator allocator = {0};

  VmaAllocatorCreateInfo allocator_info = {0};
  allocator_info.physicalDevice         = p_device->gpu->device;
  allocator_info.device                 = p_device->device;
  allocator_info.instance               = p_core->instance;
  allocator_info.flags                  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

  LBR_VK_CHECK(vmaCreateAllocator(&allocator_info, &allocator.allocator));

  return allocator;
}

void lbrDestroyGPUAllocator(LbrGPUAllocator* p_allocator) { vmaDestroyAllocator(p_allocator->allocator); }

LbrAllocatedBuffer lbrCreateAllocatedBuffer(LbrGPUAllocator* p_allocator, usize allocation_size, VkBufferUsageFlags usage) {
  LbrAllocatedBuffer buffer;

  VkBufferCreateInfo buffer_info = {0};
  buffer_info.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.size               = allocation_size;
  buffer_info.usage              = usage;

  VmaAllocationCreateInfo allocation_info = {0};
  allocation_info.usage                   = VMA_MEMORY_USAGE_AUTO;
  allocation_info.flags                   = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

  LBR_VK_CHECK(vmaCreateBuffer(p_allocator->allocator, &buffer_info, &allocation_info, &buffer.buffer, &buffer.allocation, &buffer.info));

  return buffer;
}

void lbrDestroyAllocatedBuffer(LbrAllocatedBuffer* p_buffer, LbrGPUAllocator* p_allocator) { vmaDestroyBuffer(p_allocator->allocator, p_buffer->buffer, NULL); }
