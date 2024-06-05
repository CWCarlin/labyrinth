#pragma once

// this header is cursed
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wnullability-extension"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "renderer/window.h"

/**
 * \struct
 * This struct holds the fundamental information requried for any
 * Vulkan GUI program. Any interaction with the hardware driver
 * requires an interaction at some point with the data in this struct.
 */
typedef struct lbr_vulkan_core_t {
  VkInstance instance;
  VkSurfaceKHR surface;
  LbrWindow window;
#ifdef NDEBUG
#else
  VkDebugUtilsMessengerEXT debug_messenger;
#endif
} LbrVulkanCore;

LbrVulkanCore lbrCreateVulkanCore(LbrWindowCreateInfo* p_window_info);
void lbrDestroyVulkanCore(LbrVulkanCore* p_core);
void lbrDefineVulkanDebugMessenger(LbrVulkanCore* p_core);

/**
 * \struct
 * This struct is designed to hold all the information relevant
 * to a given candidate GPU connected to the host system.
 */
typedef struct lbr_gpu_t {
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
} LbrGPU;

LbrGPU* lbrEnumerateGPUs(LbrVulkanCore* p_core, u32* num_gpus);
void lbrDestroyGPU(LbrGPU* p_gpu);
bool lbrGPUIsSuitable(LbrGPU* p_gpu, LbrVulkanCore* p_core);

/**
 * \struct
 * This struct holds a VkQueue object with family
 * and index that defines it.
 */
typedef struct lbr_queue_t {
  VkQueue queue;
  u32 queue_family;
  u32 queue_index;
} LbrQueue;

/**
 * \struct
 * This struct described a logical device that Vulkan
 * has been able to hook into and interact with. This device
 * will receive and execute commands through its command queues.
 */
typedef struct lbr_device_t {
  VkDevice device;
  LbrGPU* gpu;
  LbrQueue graphics_queue;
  LbrQueue present_queue;
  LbrQueue* compute_queues;
  LbrQueue* transfer_queues;
  u32 compute_count;
  u32 transfer_count;
} LbrDevice;

LbrDevice lbrCreateDevice(LbrVulkanCore* p_core, LbrGPU* p_gpu, u32 compute_count, u32 transfer_count);
void lbrDestroyDevice(LbrDevice* p_device);

// // frame data
// // compute pipeline
// // graphics pipeline
// // shaders
