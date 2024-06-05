#include "renderer/core.h"

#include <minwindef.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "renderer/window.h"
#include "utils/logging.h"
#include "vulkan/vulkan_core.h"

#define VALIDATION_LAYRER_COUNT  1
#define INSTANCE_EXTENSION_COUNT 5
#define DEVICE_EXTENSION_COUNT   5

static const char* g_validation_layers[VALIDATION_LAYRER_COUNT]    = {"VK_LAYER_KHRONOS_validation"};
static const char* g_instance_extensions[INSTANCE_EXTENSION_COUNT] = {VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
                                                                      VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};
static const char* g_device_extensions[DEVICE_EXTENSION_COUNT] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
                                                                  VK_KHR_DEVICE_GROUP_EXTENSION_NAME, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME};

LbrVulkanCore lbrCreateVulkanCore(LbrWindowCreateInfo* p_window_info) {
  bool validation_enabled;
#ifdef NDEBUG
  validation_enabled = false;
#else
  validation_enabled = true;
#endif

  g_instance_extensions[INSTANCE_EXTENSION_COUNT - 2] = lbrWindowGetVulkanExtensions();
  g_instance_extensions[INSTANCE_EXTENSION_COUNT - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

  LbrVulkanCore core;
  lbrDefineWindow(p_window_info, &core.window);

  VkApplicationInfo app_info  = {0};
  app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName   = p_window_info->name;
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.pEngineName        = "Labyrinth";
  app_info.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
  app_info.apiVersion         = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instance_info    = {0};
  instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo        = &app_info;
  instance_info.ppEnabledExtensionNames = g_instance_extensions;
  instance_info.enabledExtensionCount   = INSTANCE_EXTENSION_COUNT - 1;

  if (validation_enabled) {
    instance_info.enabledExtensionCount = INSTANCE_EXTENSION_COUNT;
    instance_info.enabledLayerCount     = VALIDATION_LAYRER_COUNT;
    instance_info.ppEnabledLayerNames   = g_validation_layers;
  }

  LBR_VK_CHECK(vkCreateInstance(&instance_info, NULL, &core.instance));
  lbrWindowDefineVulkanSurface(&core.window, core.instance, &core.surface);

  if (validation_enabled) {
    lbrDefineVulkanDebugMessenger(&core);
  }

  return core;
}

void lbrDestroyVulkanCore(LbrVulkanCore* p_core) {
#ifdef NDEBUG
#else
  PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
      (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_core->instance, "vkDestroyDebugUtilsMessengerEXT");
  vkDestroyDebugUtilsMessengerEXT(p_core->instance, p_core->debug_messenger, NULL);
#endif
  vkDestroySurfaceKHR(p_core->instance, p_core->surface, NULL);
  vkDestroyInstance(p_core->instance, NULL);
}

void lbrDefineVulkanDebugMessenger(LbrVulkanCore* p_core) {
#ifdef NDEBUG
  (void)p_core;
#else
  VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {0};
  debug_messenger_info.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_messenger_info.messageSeverity                    = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_messenger_info.pfnUserCallback = lbrLogVulkanValidation;

  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_core->instance, "vkCreateDebugUtilsMessengerEXT");
  LBR_VK_CHECK(vkCreateDebugMessengerEXT(p_core->instance, &debug_messenger_info, NULL, &p_core->debug_messenger));
#endif
}

LbrGPU* lbrEnumerateGPUs(LbrVulkanCore* p_core, u32* num_gpus) {
  LbrGPU* gpus;
  VkPhysicalDevice* physical_devices;
  *num_gpus = 0;

  vkEnumeratePhysicalDevices(p_core->instance, num_gpus, NULL);
  physical_devices = malloc(*num_gpus * sizeof(physical_devices));
  gpus             = malloc(*num_gpus * sizeof(LbrGPU));
  vkEnumeratePhysicalDevices(p_core->instance, num_gpus, physical_devices);

  usize suitable_count = 0;
  for (u32 i = 0; i < *num_gpus; i++) {
    LbrGPU* gpu = &gpus[i];
    gpu->device = physical_devices[i];

    vkGetPhysicalDeviceProperties(gpu->device, &gpu->properties);
    vkGetPhysicalDeviceFeatures(gpu->device, &gpu->features);
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->device, p_core->surface, &gpu->capabilities);

    u32 count;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->device, &count, NULL);
    gpu->queue_family_properties = malloc(count * sizeof(VkQueueFamilyProperties));
    gpu->queue_count             = count;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu->device, &count, gpu->queue_family_properties);

    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->device, p_core->surface, &count, NULL);
    gpu->formats      = malloc(count * sizeof(VkSurfaceFormatKHR));
    gpu->format_count = count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu->device, p_core->surface, &count, gpu->formats);

    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->device, p_core->surface, &count, NULL);
    gpu->present_modes      = malloc(count * sizeof(VkPresentModeKHR));
    gpu->present_mode_count = count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu->device, p_core->surface, &count, gpu->present_modes);

    if (lbrGPUIsSuitable(gpu, p_core)) {
      gpus[suitable_count] = *gpu;
      suitable_count++;
    } else {
      lbrDestroyGPU(gpu);
    }
  }

  LbrGPU* suitable_gpus = realloc(gpus, suitable_count * sizeof(LbrGPU));
  *num_gpus             = suitable_count;

  free(physical_devices);

  return suitable_gpus;
}

void lbrDestroyGPU(LbrGPU* p_gpu) {
  free(p_gpu->formats);
  free(p_gpu->present_modes);
  free(p_gpu->queue_family_properties);
}

bool lbrGPUIsSuitable(LbrGPU* p_gpu, LbrVulkanCore* p_core) {
  u32 count;
  VkExtensionProperties* extensions;

  vkEnumerateDeviceExtensionProperties(p_gpu->device, NULL, &count, NULL);
  extensions = malloc(count * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(p_gpu->device, NULL, &count, extensions);

  bool extension_flag;
  for (u32 i = 0; i < DEVICE_EXTENSION_COUNT; i++) {
    extension_flag = false;
    for (u32 j = 0; j < count; j++) {
      if (strcmp(extensions[j].extensionName, g_device_extensions[i]) == 0) {
        extension_flag = true;
        break;
      }
    }
    if (!extension_flag) {
      break;
    }
  }

  bool graphics_flag = false;
  bool transfer_flag = false;
  bool compute_flag  = false;
  bool present_flag  = false;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    graphics_flag |= p_gpu->queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
    transfer_flag |= p_gpu->queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT;
    compute_flag |= p_gpu->queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT;

    VkBool32 present_support;
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, i, p_core->surface, &present_support);
    present_flag |= present_support;
  }

  free(extensions);

  return extension_flag && graphics_flag && transfer_flag && compute_flag && present_flag;
}

typedef struct {
  int score;
  u32 queue_family;
  u32 queue_count;
} LbrQueueScore;

static int lbrQueueScoreSort(const void* a, const void* b) { return (((LbrQueueScore*)a)->score - ((LbrQueueScore*)b)->score); }
static void lbrDefineQueueScores(LbrVulkanCore* p_core, LbrGPU* p_gpu, LbrQueueScore* p_scores) {
  VkBool32 present_support;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, i, p_core->surface, &present_support);

    p_scores[i].queue_family = i;
    p_scores[i].score        = 0;

    if (p_gpu->queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      p_scores[i].score++;
    }

    if (p_gpu->queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
      p_scores[i].score++;
    }

    if (p_gpu->queue_family_properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
      p_scores[i].score++;
    }

    if (present_support) {
      p_scores[i].score++;
    }
  }

  qsort(p_scores, p_gpu->queue_count, sizeof(LbrQueueScore), lbrQueueScoreSort);
}

static void lbrFindMissingQueues(LbrVulkanCore* p_core, LbrGPU* p_gpu, LbrQueueScore* p_scores, LbrQueue* p_present_queue, LbrQueue* p_compute_queues, u32 computes_found,
                                 LbrQueue* p_transfer_queues, u32 transfers_found, bool present_found) {
  VkBool32 present_support;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    VkQueueFamilyProperties* prop = &p_gpu->queue_family_properties[p_scores[i].queue_family];
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, i, p_core->surface, &present_support);

    if (!present_found && present_support) {
      p_scores[i].queue_count++;
      p_present_queue->queue_family = i;
      p_present_queue->queue_index  = 0;
    }

    if (computes_found == 0 && prop->queueFlags & VK_QUEUE_COMPUTE_BIT) {
      p_scores[i].queue_count++;
      p_compute_queues[0].queue_family = i;
      p_compute_queues[0].queue_index  = 0;
    }

    if (transfers_found == 0 && prop->queueFlags & VK_QUEUE_TRANSFER_BIT) {
      p_scores[i].queue_count++;
      p_transfer_queues[0].queue_family = i;
      p_transfer_queues[0].queue_index  = 0;
    }
  }
}

static void lbrFindOptimalQueues(LbrVulkanCore* p_core, LbrGPU* p_gpu, u32* compute_count, u32* transfer_count, LbrQueueScore* p_scores, LbrQueue* p_graphics_queue,
                                 LbrQueue* p_present_queue, LbrQueue* p_compute_queues, LbrQueue* p_transfer_queues) {
  u32 computes_found  = 0;
  u32 transfers_found = 0;
  bool graphics_found = false;
  bool present_found  = false;
  VkBool32 present_support;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    VkQueueFamilyProperties* prop = &p_gpu->queue_family_properties[p_scores[i].queue_family];
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, p_scores[i].queue_family, p_core->surface, &present_support);

    for (u32 j = 0; j < prop->queueCount; j++) {
      if (!graphics_found && prop->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        p_scores[i].queue_count++;
        p_graphics_queue->queue_family = p_scores[i].queue_family;
        p_graphics_queue->queue_index  = j;
        graphics_found                 = true;
        continue;
      }

      if (!present_found && present_found) {
        p_scores[i].queue_count++;
        p_present_queue->queue_family = p_scores[i].queue_family;
        p_present_queue->queue_index  = j;
        present_found                 = true;
        continue;
      }

      if ((computes_found <= transfers_found || transfers_found == *transfer_count) && computes_found < *compute_count && prop->queueFlags & VK_QUEUE_COMPUTE_BIT) {
        p_scores[i].queue_count++;
        p_compute_queues[computes_found].queue_family = p_scores[i].queue_family;
        p_compute_queues[computes_found].queue_index  = j;
        computes_found++;
        continue;
      }

      if (transfers_found < *transfer_count && prop->queueFlags & VK_QUEUE_TRANSFER_BIT) {
        p_scores[i].queue_count++;
        p_transfer_queues[transfers_found].queue_family = p_scores[i].queue_family;
        p_transfer_queues[transfers_found].queue_index  = j;
        transfers_found++;
        continue;
      }
    }
  }

  lbrFindMissingQueues(p_core, p_gpu, p_scores, p_present_queue, p_compute_queues, computes_found, p_transfer_queues, transfers_found, present_found);

  *compute_count  = computes_found;
  *transfer_count = transfers_found;
}

#include <stdio.h>

LbrDevice lbrCreateDevice(LbrVulkanCore* p_core, LbrGPU* p_gpu, u32 compute_count, u32 transfer_count) {
  LbrDevice device = {0};

  VkPhysicalDeviceSynchronization2Features sync_2_features = {0};
  sync_2_features.sType                                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  sync_2_features.synchronization2                         = VK_TRUE;

  VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering = {0};
  dynamic_rendering.sType                                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
  dynamic_rendering.dynamicRendering                         = VK_TRUE;
  dynamic_rendering.pNext                                    = &sync_2_features;

  VkPhysicalDeviceBufferDeviceAddressFeatures buffer_device_address = {0};
  buffer_device_address.sType                                       = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
  buffer_device_address.bufferDeviceAddress                         = VK_TRUE;
  buffer_device_address.pNext                                       = &dynamic_rendering;

  LbrQueue graphics_queue;
  LbrQueue present_queue;
  LbrQueue* compute_queues  = calloc(compute_count, sizeof(LbrQueue));
  LbrQueue* transfer_queues = calloc(transfer_count, sizeof(LbrQueue));

  LbrQueueScore* scores = calloc(p_gpu->queue_count, sizeof(LbrQueueScore));
  lbrDefineQueueScores(p_core, p_gpu, scores);
  lbrFindOptimalQueues(p_core, p_gpu, &compute_count, &transfer_count, scores, &graphics_queue, &present_queue, compute_queues, transfer_queues);

  const u32 prio_length = 32;
  f32* prio             = malloc(prio_length * sizeof(f32));
  for (u32 i = 0; i < prio_length; i++) {
    prio[i] = 1.0F;
  }

  VkDeviceQueueCreateInfo* queue_info = malloc(p_gpu->queue_count * sizeof(VkDeviceQueueCreateInfo));
  u32 queue_info_count                = 0;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    if (scores[i].queue_count > 0) {
      queue_info[queue_info_count]                  = (VkDeviceQueueCreateInfo){0};
      queue_info[queue_info_count].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_info[queue_info_count].pQueuePriorities = prio;
      queue_info[queue_info_count].queueFamilyIndex = scores[i].queue_family;
      queue_info[queue_info_count].queueCount       = min(scores[i].queue_count, p_gpu->queue_family_properties[scores[i].queue_family].queueCount);
      queue_info_count++;
    }
  }

  VkDeviceCreateInfo device_info      = {0};
  device_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.pNext                   = &buffer_device_address;
  device_info.pQueueCreateInfos       = queue_info;
  device_info.queueCreateInfoCount    = queue_info_count;
  device_info.enabledExtensionCount   = DEVICE_EXTENSION_COUNT;
  device_info.ppEnabledExtensionNames = g_device_extensions;

  LBR_VK_CHECK(vkCreateDevice(p_gpu->device, &device_info, NULL, &device.device));

  device.compute_count   = compute_count;
  device.transfer_count  = transfer_count;
  device.compute_queues  = malloc(compute_count * sizeof(LbrQueue));
  device.transfer_queues = malloc(transfer_count * sizeof(LbrQueue));

  vkGetDeviceQueue(device.device, graphics_queue.queue_family, graphics_queue.queue_index, &device.graphics_queue.queue);
  vkGetDeviceQueue(device.device, present_queue.queue_family, present_queue.queue_index, &device.present_queue.queue);

  for (u32 i = 0; i < compute_count; i++) {
    device.compute_queues[i] = compute_queues[i];
    vkGetDeviceQueue(device.device, compute_queues[i].queue_family, compute_queues[i].queue_index, &device.compute_queues[i].queue);
  }

  for (u32 i = 0; i < transfer_count; i++) {
    device.transfer_queues[i] = transfer_queues[i];
    vkGetDeviceQueue(device.device, transfer_queues[i].queue_family, transfer_queues[i].queue_index, &device.transfer_queues[i].queue);
  }

  device.gpu = p_gpu;

  free(prio);
  free(queue_info);
  free(scores);
  free(compute_queues);
  free(transfer_queues);

  return device;
}

void lbrDestroyDevice(LbrDevice* p_device) {
  free(p_device->compute_queues);
  free(p_device->transfer_queues);
  vkDestroyDevice(p_device->device, NULL);
}
