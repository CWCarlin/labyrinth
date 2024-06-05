#pragma once

#include <vulkan/vulkan.h>

#include "renderer/allocator.h"
#include "renderer/core.h"
#include "renderer/descriptor.h"
#include "renderer/image.h"

/**
 * \struct
 * This struct holds the information required to
 * utilize the system swapchain, the essential component
 * for rendering to the window. The swapchain, and this struct,
 * control the render image, color space, resolution, frame rate cap,
 * and any other information used to describe the images presented
 * to the screen.
 */
typedef struct lbr_swapchain_t {
  VkSwapchainKHR swapchain;
  VkSurfaceFormatKHR format;
  VkPresentModeKHR present;
  VkExtent2D extent;
  LbrImage draw_image;
  LbrDescriptorAllocator descriptor_allocator;
  VkDescriptorSet draw_image_descriptors;
  VkDescriptorSetLayout draw_image_descriptor_layout;
  VkImage* frame_images;
  VkImageView* frame_image_views;
  u32 frame_count;
} LbrSwapchain;

LbrSwapchain lbrCreateSwapchain(LbrVulkanCore* p_core, LbrGPU* p_gpu, LbrDevice* p_device, LbrGPUAllocator* p_allocator);
void lbrDestroySwapchain(LbrSwapchain* p_swapchain, LbrDevice* p_device, LbrGPUAllocator* p_allocator);
