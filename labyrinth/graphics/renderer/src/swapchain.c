#include "renderer/swapchain.h"

#include "renderer/descriptor.h"
#include "renderer/image.h"
#include "utils/logging.h"
#include "vulkan/vulkan_core.h"

static void lbrDefineImageObjects(LbrSwapchain* p_swapchain, LbrDevice* p_device) {
  u32 count = 0;
  vkGetSwapchainImagesKHR(p_device->device, p_swapchain->swapchain, &count, NULL);
  p_swapchain->frame_images      = malloc(count * sizeof(VkImage));
  p_swapchain->frame_image_views = malloc(count * sizeof(VkImageView));
  vkGetSwapchainImagesKHR(p_device->device, p_swapchain->swapchain, &count, p_swapchain->frame_images);

  for (u32 i = 0; i < count; i++) {
    VkImageViewCreateInfo image_view_info = {0};

    image_view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.image                           = p_swapchain->frame_images[i];
    image_view_info.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.format                          = p_swapchain->format.format;
    image_view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    image_view_info.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    image_view_info.subresourceRange.baseMipLevel   = 0;
    image_view_info.subresourceRange.levelCount     = 1;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount     = 1;

    LBR_VK_CHECK(vkCreateImageView(p_device->device, &image_view_info, NULL, &p_swapchain->frame_image_views[i]));
  }
}

LbrSwapchain lbrCreateSwapchain(LbrVulkanCore* p_core, LbrGPU* p_gpu, LbrDevice* p_device, LbrGPUAllocator* p_allocator) {
  LbrSwapchain swapchain = {0};

  for (u32 i = 0; i < p_gpu->format_count; i++) {
    if (p_gpu->formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && p_gpu->formats[i].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
      swapchain.format = p_gpu->formats[i];
      break;
    }
  }

  swapchain.present = VK_PRESENT_MODE_FIFO_KHR;
  for (u32 i = 0; i < p_gpu->present_mode_count; i++) {
    if (p_gpu->present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
      swapchain.present = p_gpu->present_modes[i];
      break;
    }
  }

  if (p_gpu->capabilities.currentExtent.width != UINT32_MAX) {
    swapchain.extent = p_gpu->capabilities.currentExtent;
  } else {
    swapchain.extent        = p_core->window.resolution;
    swapchain.extent.width  = swapchain.extent.width < p_device->gpu->capabilities.minImageExtent.width ? p_device->gpu->capabilities.minImageExtent.width
                                                                                                        : p_device->gpu->capabilities.maxImageExtent.width;
    swapchain.extent.width  = swapchain.extent.width > p_device->gpu->capabilities.maxImageExtent.width ? p_device->gpu->capabilities.maxImageExtent.width : swapchain.extent.width;
    swapchain.extent.height = swapchain.extent.height < p_device->gpu->capabilities.minImageExtent.height ? p_device->gpu->capabilities.minImageExtent.height
                                                                                                          : p_device->gpu->capabilities.maxImageExtent.height;
    swapchain.extent.height =
        swapchain.extent.height > p_device->gpu->capabilities.maxImageExtent.height ? p_device->gpu->capabilities.maxImageExtent.height : swapchain.extent.height;
  }

  swapchain.frame_count = p_device->gpu->capabilities.minImageCount + 1;
  if (p_device->gpu->capabilities.maxImageCount > 0 && swapchain.frame_count > p_device->gpu->capabilities.maxImageCount) {
    swapchain.frame_count = p_device->gpu->capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR swapchain_info = {0};
  swapchain_info.sType                    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchain_info.surface                  = p_core->surface;
  swapchain_info.minImageCount            = swapchain.frame_count;
  swapchain_info.imageFormat              = swapchain.format.format;
  swapchain_info.imageColorSpace          = swapchain.format.colorSpace;
  swapchain_info.imageExtent              = swapchain.extent;
  swapchain_info.imageArrayLayers         = 1;
  swapchain_info.imageUsage               = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  swapchain_info.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
  swapchain_info.preTransform             = p_device->gpu->capabilities.currentTransform;
  swapchain_info.compositeAlpha           = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchain_info.presentMode              = swapchain.present;
  swapchain_info.clipped                  = VK_TRUE;
  swapchain_info.oldSwapchain             = VK_NULL_HANDLE;

  LBR_VK_CHECK(vkCreateSwapchainKHR(p_device->device, &swapchain_info, NULL, &swapchain.swapchain));
  lbrDefineImageObjects(&swapchain, p_device);
  swapchain.draw_image = lbrCreateImage(p_core, p_device, p_allocator);

  LbrPoolRatio ratio = {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0F};

  swapchain.descriptor_allocator = lbrCreateDescriptorAllocator(10, 1, &ratio, 1, 0);
  lbrDescriptorAllocatorAllocatePool(&swapchain.descriptor_allocator, p_device);

  LbrDescriptorLayout layout = lbrCreateDescriptorLayout(1);
  lbrAddDescriptorBinding(&layout, 0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  swapchain.draw_image_descriptor_layout = lbrBuildDescriptorLayout(&layout, p_device, VK_SHADER_STAGE_COMPUTE_BIT);
  swapchain.draw_image_descriptors       = lbrCreateDescriptorSet(&swapchain.descriptor_allocator, p_device, swapchain.draw_image_descriptor_layout);
  lbrDestroyDescriptorLayout(&layout);

  LbrDescriptorWriter writer = lbrCreateDescriptorWriter(1);
  lbrDescriptorWriterWriteImage(&writer, 0, &swapchain.draw_image, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
  lbrDescriptorWriterUpdateSet(&writer, p_device, swapchain.draw_image_descriptors);
  lbrDestroyDescriptorWriter(&writer);

  return swapchain;
}

void lbrDestroySwapchain(LbrSwapchain* p_swapchain, LbrDevice* p_device, LbrGPUAllocator* p_allocator) {
  for (u32 i = 0; i < p_swapchain->frame_count; i++) {
    vkDestroyImageView(p_device->device, p_swapchain->frame_image_views[i], NULL);
    vkDestroyImage(p_device->device, p_swapchain->frame_images[i], NULL);
  }
  free(p_swapchain->frame_images);
  free(p_swapchain->frame_image_views);

  vkDestroyDescriptorSetLayout(p_device->device, p_swapchain->draw_image_descriptor_layout, NULL);
  lbrDestroyDescriptorAllocator(&p_swapchain->descriptor_allocator, p_device);
  lbrDestroyImage(&p_swapchain->draw_image, p_device, p_allocator);
  vkDestroySwapchainKHR(p_device->device, p_swapchain->swapchain, NULL);
}
