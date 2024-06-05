#pragma once

#include <vulkan/vulkan.h>

#include "renderer/allocator.h"
#include "renderer/core.h"

/**
 * \struct
 * This struct describes a generic image that can be read to, written by
 * or presented by the Vulkan API.
 */
typedef struct lbr_image_t {
  VkImage image;
  VkImageView image_view;
  VmaAllocation allocation;
  VkExtent3D extent;
  VkFormat format;
} LbrImage;

LbrImage lbrCreateImage(LbrVulkanCore* p_core, LbrDevice* p_device, LbrGPUAllocator* p_allocator);
void lbrDestroyImage(LbrImage* p_image, LbrDevice* p_device, LbrGPUAllocator* p_allocator);
VkImageCreateInfo lbrCreateImageInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent);
VkImageViewCreateInfo lbrCreateImageViewInfo(VkFormat format, LbrImage* p_image, VkImageAspectFlags aspect_mask);
VkImageSubresourceRange lbrCreateImageSubresourceRange(VkImageAspectFlags aspect_mask);
void lbrImageTransition(VkCommandBuffer command_buffer, LbrImage* image, VkImageLayout current_layout, VkImageLayout new_layout);
void lbrImageToImageCopy(VkCommandBuffer command_buffer, LbrImage* source, LbrImage* destination);
