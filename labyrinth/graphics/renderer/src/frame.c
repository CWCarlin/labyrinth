#include "renderer/frame.h"

#include "utils/logging.h"
#include "vulkan/vulkan_core.h"

LbrCommandList lbrCreateCommandList(LbrDevice* p_device, u32 queue_family) {
  LbrCommandList command_list  = {0};
  VkCommandPoolCreateInfo info = {0};
  info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  info.flags                   = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  info.queueFamilyIndex        = queue_family;
  vkCreateCommandPool(p_device->device, &info, NULL, &command_list.command_pool);

  VkCommandBufferAllocateInfo command_buffer_info = {0};
  command_buffer_info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  command_buffer_info.commandPool                 = command_list.command_pool;
  command_buffer_info.commandBufferCount          = 20;
  command_buffer_info.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  vkAllocateCommandBuffers(p_device->device, &command_buffer_info, command_list.command_buffers);
  return command_list;
}

void lbrDestroyCommandList(LbrCommandList* p_list, LbrDevice* p_device) { vkDestroyCommandPool(p_device->device, p_list->command_pool, NULL); }

void lbrResetCommandList(LbrCommandList* p_list, LbrDevice* p_device) {
  vkResetCommandPool(p_device->device, p_list->command_pool, 0);
  p_list->command_buffer_count = 0;
}

VkCommandBuffer lbrCommandListGetBuffer(LbrCommandList* p_list) {
  LBR_ASSERT(p_list->command_buffer_count != 20);
  p_list->command_buffer_count++;
  return p_list->command_buffers[p_list->command_buffer_count - 1];
}

LbrFrameData lbrCreateFrameData(LbrDevice* p_device) {
  LbrFrameData frame_data = {0};

  frame_data.compute_list  = lbrCreateCommandList(p_device, p_device->compute_queues->queue_family);
  frame_data.graphics_list = lbrCreateCommandList(p_device, p_device->graphics_queue.queue_family);
  frame_data.transfer_list = lbrCreateCommandList(p_device, p_device->transfer_queues->queue_family);

  VkSemaphoreCreateInfo semaphore_info = {0};
  semaphore_info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  vkCreateSemaphore(p_device->device, &semaphore_info, NULL, &frame_data.swapchain_semaphore);
  vkCreateSemaphore(p_device->device, &semaphore_info, NULL, &frame_data.render_semaphore);

  VkFenceCreateInfo fence_info = {0};
  fence_info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags             = VK_FENCE_CREATE_SIGNALED_BIT;
  vkCreateFence(p_device->device, &fence_info, NULL, &frame_data.render_fence);

  return frame_data;
}

void lbrDestroyFrameData(LbrFrameData* p_data, LbrDevice* p_device) {
  lbrDestroyCommandList(&p_data->compute_list, p_device);
  lbrDestroyCommandList(&p_data->graphics_list, p_device);
  lbrDestroyCommandList(&p_data->transfer_list, p_device);
  vkDestroySemaphore(p_device->device, p_data->swapchain_semaphore, NULL);
  vkDestroySemaphore(p_device->device, p_data->render_semaphore, NULL);
  vkDestroyFence(p_device->device, p_data->render_fence, NULL);
}

void lbrResetFrameData(LbrFrameData* p_data, LbrDevice* p_device) {
  lbrResetCommandList(&p_data->compute_list, p_device);
  lbrResetCommandList(&p_data->graphics_list, p_device);
  lbrResetCommandList(&p_data->transfer_list, p_device);
  vkResetFences(p_device->device, 1, &p_data->render_fence);
}

VkCommandBuffer lbrFrameDataGetBuffer(LbrFrameData* p_data, enum LbrCommandType type) {
  VkCommandBuffer buffer;

  switch (type) {
    case LBR_COMPUTE_COMMAND:
      buffer = lbrCommandListGetBuffer(&p_data->compute_list);
      break;
    case LBR_GRAPHICS_COMMAND:
      buffer = lbrCommandListGetBuffer(&p_data->graphics_list);
      break;
    case LBR_TRANSFER_COMMAND:
      buffer = lbrCommandListGetBuffer(&p_data->transfer_list);
      break;
  }

  return buffer;
}
