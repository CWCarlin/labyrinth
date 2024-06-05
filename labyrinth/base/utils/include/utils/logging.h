#pragma once

#ifdef LOGGING
#include "utils/types.h"
#include "vulkan/vk_platform.h"
#include "vulkan/vulkan_core.h"

VKAPI_ATTR VkBool32 lbrLogVulkanValidation(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT type,
                                           const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* data);

void lbrLogAssertionFail(const char* expr, const char* file, u32 line);
void lbrLogInfo(const char* message, const char* file, u32 line);
void lbrLogWarning(const char* message, const char* file, u32 line);
void lbrLogError(const char* message, const char* file, u32 line);
void lbrLogVulkanFailure(const char* expr, VkResult result, const char* file, u32 line);

#define lbrDebugBreak() __asm("int3")
#define LBR_ASSERT(expr)                                                                                                                                                           \
  if (expr) {                                                                                                                                                                      \
  } else {                                                                                                                                                                         \
    lbrLogAssertionFail(#expr, __FILE__, __LINE__);                                                                                                                                \
    lbrDebugBreak();                                                                                                                                                               \
  }

#define LBR_LOG_INFO(message) lbrLogInfo(#message, __FILE__, __LINE__)
#define LBR_LOG_WARN(message) lbrLogWarning(#message, __FILE__, __LINE__)
#define LBR_LOG_ERROR(message)                                                                                                                                                     \
  lbrLogError(#message, __FILE__, __LINE__);                                                                                                                                       \
  lbrDebugBreak()

#define LBR_VK_CHECK(expr)                                                                                                                                                         \
  VkResult res = expr;                                                                                                                                                             \
  if (res == VK_SUCCESS) {                                                                                                                                                         \
  } else {                                                                                                                                                                         \
    lbrLogVulkanFailure(#expr, res, __FILE__, __LINE__);                                                                                                                           \
    lbrDebugBreak();                                                                                                                                                               \
  }

#else
#define LBR_ASSERT(expr)
#define LBR_LOG_INFO(message)
#define LBR_LOG_WARN(message)
#define LBR_LOG_ERROR(message)
#define LBR_VK_CHECK(expr)
#endif
