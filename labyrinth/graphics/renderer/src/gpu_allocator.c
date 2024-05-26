#include "renderer/gpu_allocator.h"

#include "renderer/core.h"

LbrDeviceAllocator lbrCreateDeviceAllocator(LbrVulkanCore* p_core, LbrLogicalDevice* p_device) {
  LbrDeviceAllocator allocator = {0};

  VmaAllocatorCreateInfo allocator_info = {0};
  allocator_info.physicalDevice         = p_device->gpu.device;
  allocator_info.device                 = p_device->device;
  allocator_info.instance               = p_core->instance;
  allocator_info.flags                  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
  vmaCreateAllocator(&allocator_info, &allocator.allocator);

  return allocator;
}
