#pragma once

#include <vulkan/vulkan.h>

#include "renderer/core.h"

enum LbrCommandType {
  LBR_COMPUTE_COMMAND,
  LBR_GRAPHICS_COMMAND,
  LBR_TRANSFER_COMMAND,
};

typedef struct lbr_command_list_t {
  VkCommandPool command_pool;
  VkCommandBuffer command_buffers[20];
  u32 command_buffer_count;
} LbrCommandList;

LbrCommandList lbrCreateCommandList(LbrDevice* p_device, u32 queue_family);
void lbrDestroyCommandList(LbrCommandList* p_list, LbrDevice* p_device);
void lbrResetCommandList(LbrCommandList* p_list, LbrDevice* p_device);
VkCommandBuffer lbrCommandListGetBuffer(LbrCommandList* p_list);

typedef struct lbr_frame_data {
  LbrCommandList compute_list;
  LbrCommandList graphics_list;
  LbrCommandList transfer_list;
  VkSemaphore swapchain_semaphore;
  VkSemaphore render_semaphore;
  VkFence render_fence;
} LbrFrameData;

LbrFrameData lbrCreateFrameData(LbrDevice* p_device);
void lbrDestroyFrameData(LbrFrameData* p_data, LbrDevice* p_device);
void lbrResetFrameData(LbrFrameData* p_data, LbrDevice* p_device);
VkCommandBuffer lbrFrameDataGetBuffer(LbrFrameData* p_data, enum LbrCommandType type);
