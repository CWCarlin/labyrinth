#include "renderer/image.h"

#include <vulkan/vulkan_core.h>

#include "renderer/allocator.h"
#include "renderer/core.h"
#include "utils/logging.h"

LbrImage lbrCreateImage(LbrVulkanCore* p_core, LbrDevice* p_device, LbrGPUAllocator* p_allocator) {
  LbrImage image;
  image.format = VK_FORMAT_R16G16B16A16_SFLOAT;
  image.extent = (VkExtent3D){p_core->window.resolution.width, p_core->window.resolution.height, 1};

  VkImageUsageFlags image_usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  VkImageCreateInfo image_info  = lbrCreateImageInfo(image.format, image_usage, image.extent);

  VmaAllocationCreateInfo allocation_info = {0};
  allocation_info.usage                   = VMA_MEMORY_USAGE_AUTO;
  allocation_info.flags                   = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
  allocation_info.requiredFlags           = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  allocation_info.priority                = 1.0F;

  vmaCreateImage(p_allocator->allocator, &image_info, &allocation_info, &image.image, &image.allocation, NULL);

  VkImageViewCreateInfo image_view_info = lbrCreateImageViewInfo(image.format, &image, VK_IMAGE_ASPECT_COLOR_BIT);
  LBR_VK_CHECK(vkCreateImageView(p_device->device, &image_view_info, NULL, &image.image_view));

  return image;
}

void lbrDestroyImage(LbrImage* p_image, LbrDevice* p_device, LbrGPUAllocator* p_allocator) {
  vkDestroyImageView(p_device->device, p_image->image_view, NULL);
  vmaDestroyImage(p_allocator->allocator, p_image->image, p_image->allocation);
}

VkImageCreateInfo lbrCreateImageInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D extent) {
  VkImageCreateInfo image_info = {0};
  image_info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.imageType         = VK_IMAGE_TYPE_2D;
  image_info.format            = format;
  image_info.extent            = extent;
  image_info.mipLevels         = 1;
  image_info.arrayLayers       = 1;
  image_info.samples           = VK_SAMPLE_COUNT_1_BIT;
  image_info.tiling            = VK_IMAGE_TILING_OPTIMAL;
  image_info.usage             = usage_flags;

  return image_info;
}

VkImageViewCreateInfo lbrCreateImageViewInfo(VkFormat format, LbrImage* p_image, VkImageAspectFlags aspect_mask) {
  VkImageViewCreateInfo image_view_info           = {0};
  image_view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  image_view_info.image                           = p_image->image;
  image_view_info.format                          = format;
  image_view_info.subresourceRange.aspectMask     = aspect_mask;
  image_view_info.subresourceRange.baseMipLevel   = 0;
  image_view_info.subresourceRange.levelCount     = 1;
  image_view_info.subresourceRange.baseArrayLayer = 0;
  image_view_info.subresourceRange.layerCount     = 1;

  return image_view_info;
}

VkImageSubresourceRange lbrCreateImageSubresourceRange(VkImageAspectFlags aspect_mask) {
  VkImageSubresourceRange image_subresource = {0};
  image_subresource.aspectMask              = aspect_mask;
  image_subresource.baseMipLevel            = 0;
  image_subresource.levelCount              = VK_REMAINING_MIP_LEVELS;
  image_subresource.baseArrayLayer          = 0;
  image_subresource.layerCount              = VK_REMAINING_ARRAY_LAYERS;

  return image_subresource;
}

void lbrImageTransition(VkCommandBuffer command_buffer, LbrImage* p_image, VkImageLayout current_layout, VkImageLayout new_layout) {
  VkImageMemoryBarrier2 image_memory_barrier = {0};
  image_memory_barrier.sType                 = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
  image_memory_barrier.image                 = p_image->image;
  image_memory_barrier.srcStageMask          = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  image_memory_barrier.srcAccessMask         = VK_ACCESS_2_MEMORY_WRITE_BIT;
  image_memory_barrier.dstStageMask          = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
  image_memory_barrier.dstAccessMask         = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
  image_memory_barrier.oldLayout             = current_layout;
  image_memory_barrier.newLayout             = new_layout;

  VkImageAspectFlags aspect_mask        = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
  image_memory_barrier.subresourceRange = lbrCreateImageSubresourceRange(aspect_mask);

  VkDependencyInfo dependency_info        = {0};
  dependency_info.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
  dependency_info.imageMemoryBarrierCount = 1;
  dependency_info.pImageMemoryBarriers    = &image_memory_barrier;

  vkCmdPipelineBarrier2(command_buffer, &dependency_info);
}

void lbrImageToImageCopy(VkCommandBuffer command_buffer, LbrImage* source, LbrImage* destination) {
  VkImageBlit2 image_blit                  = {0};
  image_blit.sType                         = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
  image_blit.srcOffsets[1].x               = (i32)source->extent.width;
  image_blit.srcOffsets[1].y               = (i32)source->extent.height;
  image_blit.srcOffsets[1].z               = (i32)source->extent.depth;
  image_blit.dstOffsets[1].x               = (i32)destination->extent.width;
  image_blit.dstOffsets[1].y               = (i32)destination->extent.height;
  image_blit.dstOffsets[1].z               = (i32)destination->extent.depth;
  image_blit.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  image_blit.srcSubresource.baseArrayLayer = 0;
  image_blit.srcSubresource.layerCount     = 1;
  image_blit.srcSubresource.mipLevel       = 0;
  image_blit.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  image_blit.dstSubresource.baseArrayLayer = 0;
  image_blit.dstSubresource.layerCount     = 1;
  image_blit.dstSubresource.mipLevel       = 0;

  VkBlitImageInfo2 blit_image_info = {0};
  blit_image_info.sType            = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
  blit_image_info.dstImage         = destination->image;
  blit_image_info.dstImageLayout   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  blit_image_info.srcImage         = source->image;
  blit_image_info.srcImageLayout   = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
  blit_image_info.filter           = VK_FILTER_LINEAR;
  blit_image_info.regionCount      = 1;
  blit_image_info.pRegions         = &image_blit;

  vkCmdBlitImage2(command_buffer, &blit_image_info);
}
