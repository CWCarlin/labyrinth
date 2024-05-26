#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "renderer/window.h"

/*
 * This struct holds the fundamental information for any Vulkan GUI program.
 */
typedef struct lbr_vulkan_core_t {
  VkInstance instance;
  VkSurfaceKHR surface;
  VkDebugUtilsMessengerEXT debug_messenger;
} LbrVulkanCore;

LbrVulkanCore lbrCreateVulkanCore(LbrWindow* p_window, lbr_bool enable_validation);
void lbrDefineVulkanDebugMessenger(LbrVulkanCore* p_core);
void lbrDestroyVulkanCore(LbrVulkanCore* p_core);

/*
 * This struct defines the information held within a graphics device
 * that Vulkan can hook into and use.
 */
typedef struct lbr_connected_gpu_t {
  VkPhysicalDevice device;
  VkPhysicalDeviceProperties properties;
  VkPhysicalDeviceFeatures features;
  VkSurfaceCapabilitiesKHR capabilities;
  VkSurfaceFormatKHR* formats;
  VkPresentModeKHR* present_modes;
  VkQueueFamilyProperties* queue_family_properties;
  u32 format_count;
  u32 present_mode_count;
  u32 queue_count;
} LbrConnectedGPU;

/*
 * This struct just wraps a collection of connected GPUs
 * that the program considers to be suitable for its
 * render uses.
 */
typedef struct lbr_suitable_gpus_t {
  LbrConnectedGPU* gpu;
  usize count;
} LbrSuitableGPUs;

LbrSuitableGPUs lbrEnumerateConnectedGPUs(LbrVulkanCore* p_core);
void lbrDestroyConnectedGPU(LbrConnectedGPU* p_gpu);
lbr_bool lbrConnectedGPUIsSuitable(LbrConnectedGPU* p_gpu, LbrVulkanCore* p_core);

/*
 * This struct defines a fully defined and hooked into device
 * as well as its associated command queues.
 */
typedef struct lbr_logical_device_t {
  VkDevice device;
  LbrConnectedGPU gpu;
  VkQueue graphics_queue;
  VkQueue present_queue;
  VkQueue* compute_queues;
  VkQueue* transfer_queues;
  u32 compute_count;
  u32 transfer_count;
} LbrLogicalDevice;

LbrLogicalDevice lbrCreateLogicalDevice(LbrVulkanCore* p_core, LbrConnectedGPU* p_gpu, usize queue_count);
void lbrDestroyLogicalDevice(LbrLogicalDevice* p_logical_device);
