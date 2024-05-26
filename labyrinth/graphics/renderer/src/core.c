#include "renderer/core.h"

#include <stdlib.h>
#include <string.h>

#include "renderer/window.h"
#include "utils/logging.h"
#include "utils/types.h"
#include "vulkan/vulkan_core.h"

#define VALIDATION_LAYER_COUNT   1
#define INSTANCE_EXTENSION_COUNT 5
#define DEVICE_EXTENSION_COUNT   5

static const char* device_extensions[DEVICE_EXTENSION_COUNT] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
    VK_KHR_DEVICE_GROUP_EXTENSION_NAME, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME};

LbrVulkanCore lbrCreateVulkanCore(LbrWindow* p_window, lbr_bool enable_validation) {
  LbrVulkanCore core = {0};

  const char* instance_extensions[INSTANCE_EXTENSION_COUNT];
  instance_extensions[0] = lbrWindowGetVulkanExtensions();
  instance_extensions[1] = VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME;
  instance_extensions[2] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
  instance_extensions[3] = VK_KHR_SURFACE_EXTENSION_NAME;

  VkApplicationInfo app_info  = {0};
  app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pApplicationName   = "Labyrinth";
  app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  app_info.pEngineName        = "Labyrinth";
  app_info.engineVersion      = VK_MAKE_VERSION(0, 0, 1);
  app_info.apiVersion         = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instance_info    = {0};
  instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_info.pApplicationInfo        = &app_info;
  instance_info.enabledExtensionCount   = INSTANCE_EXTENSION_COUNT - 1;
  instance_info.ppEnabledExtensionNames = instance_extensions;

  if (enable_validation) {
    const char* validation_layers[1] = {"VK_LAYER_KHRONOS_validation"};

    instance_extensions[INSTANCE_EXTENSION_COUNT - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    instance_info.enabledLayerCount                   = VALIDATION_LAYER_COUNT;
    instance_info.ppEnabledLayerNames                 = validation_layers;
    instance_info.enabledExtensionCount++;
  }

  LBR_ASSERT(vkCreateInstance(&instance_info, NULL, &core.instance) == VK_SUCCESS);
  if (enable_validation) {
    lbrDefineVulkanDebugMessenger(&core);
  }
  lbrWindowDefineVulkanSurface(p_window, core.instance, &core.surface);

  return core;
}

void lbrDefineVulkanDebugMessenger(LbrVulkanCore* p_core) {
  VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = {0};
  debug_messenger_info.sType                              = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debug_messenger_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debug_messenger_info.pfnUserCallback = lbrLogVulkanValidation;

  PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugMessengerEXT =
      (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_core->instance, "vkCreateDebugUtilsMessengerEXT");
  LBR_ASSERT(vkCreateDebugMessengerEXT(p_core->instance, &debug_messenger_info, NULL, &p_core->debug_messenger) == VK_SUCCESS);
}

void lbrDestroyVulkanCore(LbrVulkanCore* p_core) {
  if (p_core->debug_messenger) {
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_core->instance, "vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(p_core->instance, p_core->debug_messenger, NULL);
  }

  vkDestroySurfaceKHR(p_core->instance, p_core->surface, NULL);
  vkDestroyInstance(p_core->instance, NULL);
}

LbrSuitableGPUs lbrEnumerateConnectedGPUs(LbrVulkanCore* p_core) {
  LbrConnectedGPU* connected_gpus    = NULL;
  VkPhysicalDevice* physical_devices = NULL;
  u32 device_count                   = 0;

  vkEnumeratePhysicalDevices(p_core->instance, &device_count, NULL);
  physical_devices = malloc(device_count * sizeof(VkPhysicalDevice));
  vkEnumeratePhysicalDevices(p_core->instance, &device_count, physical_devices);

  connected_gpus       = malloc(device_count * sizeof(LbrConnectedGPU));
  usize suitable_count = 0;
  for (usize i = 0; i < device_count; i++) {
    LbrConnectedGPU* gpu = &connected_gpus[i];
    u32 count            = 0;
    gpu->device          = physical_devices[i];
    vkGetPhysicalDeviceProperties(gpu->device, &gpu->properties);
    vkGetPhysicalDeviceFeatures(gpu->device, &gpu->features);
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu->device, p_core->surface, &gpu->capabilities);

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

    if (lbrConnectedGPUIsSuitable(gpu, p_core)) {
      connected_gpus[suitable_count] = *gpu;
      suitable_count++;
    } else {
      lbrDestroyConnectedGPU(gpu);
    }
  }

  LbrConnectedGPU* suitable_gpus      = realloc(connected_gpus, suitable_count * sizeof(LbrConnectedGPU));
  LbrSuitableGPUs suitable_collection = {suitable_gpus, suitable_count};

  free(physical_devices);

  return suitable_collection;
}

void lbrDestroyConnectedGPU(LbrConnectedGPU* p_gpu) {
  free(p_gpu->formats);
  free(p_gpu->present_modes);
  free(p_gpu->queue_family_properties);
}

lbr_bool lbrConnectedGPUIsSuitable(LbrConnectedGPU* p_gpu, LbrVulkanCore* p_core) {
  u32 count;
  VkExtensionProperties* extensions;
  vkEnumerateDeviceExtensionProperties(p_gpu->device, NULL, &count, NULL);
  extensions = malloc(count * sizeof(VkExtensionProperties));
  vkEnumerateDeviceExtensionProperties(p_gpu->device, NULL, &count, extensions);

  lbr_bool extension_flag;
  for (u32 i = 0; i < DEVICE_EXTENSION_COUNT; i++) {
    extension_flag = LBR_FALSE;
    for (u32 j = 0; j < count; j++) {
      if (strcmp(extensions[j].extensionName, device_extensions[i]) == 0) {
        extension_flag = LBR_TRUE;
        break;
      }
    }
    if (!extension_flag) {
      break;
    }
  }

  lbr_bool graphics_flag = LBR_FALSE;
  lbr_bool transfer_flag = LBR_FALSE;
  lbr_bool compute_flag  = LBR_FALSE;
  lbr_bool present_flag  = LBR_FALSE;
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
  u32 queue_index;
  u32 queue_count;
} LbrQueueScore;

typedef struct {
  u32 queue_family;
  u32 queue_index;
} LbrQueueFamily;

static int lbrQueueScoreSort(const void* a, const void* b) { return (((LbrQueueScore*)a)->score - ((LbrQueueScore*)b)->score); }

static void lbrDefineQueueScores(LbrVulkanCore* p_core, LbrConnectedGPU* p_gpu, LbrQueueScore* p_scores) {
  VkBool32 present_support;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, i, p_core->surface, &present_support);

    p_scores[i].queue_index = i;
    p_scores[i].score       = 0;

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

static void lbrFindMisingQueues(LbrVulkanCore* p_core, LbrConnectedGPU* p_gpu, LbrQueueScore* p_scores,
                                LbrQueueFamily* p_present_queue, LbrQueueFamily* p_compute_queues, const u32* p_computes_found,
                                LbrQueueFamily* p_transfer_queues, const u32* p_transfers_found, lbr_bool present_found) {
  VkBool32 present_support;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    VkQueueFamilyProperties* prop = &p_gpu->queue_family_properties[p_scores[i].queue_index];
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, i, p_core->surface, &present_support);

    if (!present_found && present_support) {
      p_scores[i].queue_count++;
      p_present_queue->queue_family = i;
      p_present_queue->queue_index  = 0;
    }

    if (*p_computes_found == 0 && prop->queueFlags & VK_QUEUE_COMPUTE_BIT) {
      p_scores[i].queue_count++;
      p_compute_queues[0].queue_family = i;
      p_compute_queues[0].queue_index  = 0;
    }

    if (*p_transfers_found == 0 && prop->queueFlags & VK_QUEUE_TRANSFER_BIT) {
      p_scores[i].queue_count++;
      p_transfer_queues[0].queue_family = i;
      p_transfer_queues[0].queue_index  = 0;
    }
  }
}

static void lbrFindOptimalQueues(LbrVulkanCore* p_core, LbrConnectedGPU* p_gpu, u32 queue_count, LbrQueueScore* p_scores,
                                 LbrQueueFamily* p_graphics_queue, LbrQueueFamily* p_present_queue,
                                 LbrQueueFamily* p_compute_queues, u32* p_computes_found, LbrQueueFamily* p_transfer_queues,
                                 u32* p_transfers_found) {
  lbr_bool graphics_found = LBR_FALSE;
  lbr_bool present_found  = LBR_FALSE;
  VkBool32 present_support;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    VkQueueFamilyProperties* prop = &p_gpu->queue_family_properties[p_scores[i].queue_index];
    vkGetPhysicalDeviceSurfaceSupportKHR(p_gpu->device, p_scores[i].queue_index, p_core->surface, &present_support);

    for (u32 j = 0; j < prop->queueCount; j++) {
      if (!graphics_found && prop->queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        p_scores[i].queue_count++;
        p_graphics_queue->queue_family = p_scores[i].queue_index;
        p_graphics_queue->queue_index  = j;
        graphics_found                 = LBR_TRUE;
        continue;
      }

      if (!present_found && present_support) {
        p_scores[i].queue_count++;
        p_present_queue->queue_family = p_scores[i].queue_index;
        p_present_queue->queue_index  = j;
        present_found                 = LBR_TRUE;
        continue;
      }

      if (*p_computes_found <= *p_transfers_found && *p_computes_found < queue_count && prop->queueFlags & VK_QUEUE_COMPUTE_BIT) {
        p_scores[i].queue_count++;
        p_compute_queues[*p_computes_found].queue_family = p_scores[i].queue_index;
        p_compute_queues[*p_computes_found].queue_index  = j;
        (*p_computes_found)++;
        continue;
      }

      if (*p_transfers_found < queue_count && prop->queueFlags & VK_QUEUE_TRANSFER_BIT) {
        p_scores[i].queue_count++;
        p_transfer_queues[*p_transfers_found].queue_family = p_scores[i].queue_index;
        p_transfer_queues[*p_transfers_found].queue_index  = j;
        (*p_transfers_found)++;
        continue;
      }
    }
  }

  lbrFindMisingQueues(p_core, p_gpu, p_scores, p_present_queue, p_compute_queues, p_computes_found, p_transfer_queues,
                      p_transfers_found, present_found);
}

LbrLogicalDevice lbrCreateLogicalDevice(LbrVulkanCore* p_core, LbrConnectedGPU* p_gpu, usize queue_count) {
  LbrLogicalDevice logical_device                          = {0};
  VkPhysicalDeviceSynchronization2Features sync_2_features = {0};
  sync_2_features.sType                                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
  sync_2_features.synchronization2                         = VK_TRUE;
  sync_2_features.pNext                                    = NULL;

  VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering = {0};
  dynamic_rendering.sType                                    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
  dynamic_rendering.dynamicRendering                         = VK_TRUE;
  dynamic_rendering.pNext                                    = &sync_2_features;

  VkPhysicalDeviceBufferDeviceAddressFeaturesKHR buffer_device_address = {0};
  buffer_device_address.sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
  buffer_device_address.bufferDeviceAddress = VK_TRUE;
  buffer_device_address.pNext               = &dynamic_rendering;

  LbrQueueFamily graphics_queue;
  LbrQueueFamily present_queue;
  LbrQueueFamily* compute_queues  = calloc(queue_count, sizeof(LbrQueueFamily));
  LbrQueueFamily* transfer_queues = calloc(queue_count, sizeof(LbrQueueFamily));
  u32 computes_found              = 0;
  u32 transfers_found             = 0;

  LbrQueueScore* scores = calloc(p_gpu->queue_count, sizeof(LbrQueueScore));
  lbrDefineQueueScores(p_core, p_gpu, scores);

  lbrFindOptimalQueues(p_core, p_gpu, queue_count, scores, &graphics_queue, &present_queue, compute_queues, &computes_found,
                       transfer_queues, &transfers_found);

  const u32 priority_length = 100;

  f32* priority = malloc(priority_length * sizeof(f32));
  for (u32 i = 0; i < priority_length; i++) {
    priority[i] = 1.0F;
  }

  VkDeviceQueueCreateInfo* queue_info = malloc(p_gpu->queue_count * sizeof(VkDeviceQueueCreateInfo));
  u32 queue_info_count                = 0;
  for (u32 i = 0; i < p_gpu->queue_count; i++) {
    if (scores[i].queue_count > 0) {
      queue_info[queue_info_count]                  = (VkDeviceQueueCreateInfo){0};
      queue_info[queue_info_count].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queue_info[queue_info_count].pQueuePriorities = priority;
      queue_info[queue_info_count].queueFamilyIndex = scores[i].queue_index;
      queue_info[queue_info_count].queueCount       = scores[i].queue_count;
      queue_info_count++;
    }
  }

  VkDeviceCreateInfo device_info      = {0};
  device_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  device_info.pNext                   = &buffer_device_address;
  device_info.pQueueCreateInfos       = queue_info;
  device_info.queueCreateInfoCount    = queue_info_count;
  device_info.enabledExtensionCount   = DEVICE_EXTENSION_COUNT;
  device_info.ppEnabledExtensionNames = device_extensions;

  vkCreateDevice(p_gpu->device, &device_info, NULL, &logical_device.device);

  logical_device.compute_count   = computes_found;
  logical_device.transfer_count  = transfers_found;
  logical_device.compute_queues  = malloc(computes_found * sizeof(VkQueue));
  logical_device.transfer_queues = malloc(transfers_found * sizeof(VkQueue));

  vkGetDeviceQueue(logical_device.device, graphics_queue.queue_family, graphics_queue.queue_index,
                   &logical_device.graphics_queue);
  vkGetDeviceQueue(logical_device.device, present_queue.queue_family, present_queue.queue_family, &logical_device.present_queue);

  for (u32 i = 0; i < computes_found; i++) {
    vkGetDeviceQueue(logical_device.device, compute_queues[i].queue_family, compute_queues[i].queue_index,
                     &logical_device.compute_queues[i]);
  }

  for (u32 i = 0; i < transfers_found; i++) {
    vkGetDeviceQueue(logical_device.device, transfer_queues[i].queue_family, transfer_queues[i].queue_index,
                     &logical_device.transfer_queues[i]);
  }

  logical_device.gpu = *p_gpu;

  free(scores);
  free(compute_queues);
  free(transfer_queues);
  free(priority);
  free(queue_info);

  return logical_device;
}

void lbrDestroyLogicalDevice(LbrLogicalDevice* p_logical_device) {
  free(p_logical_device->compute_queues);
  free(p_logical_device->transfer_queues);
  vkDestroyDevice(p_logical_device->device, NULL);
}
