#pragma once

#include <stdbool.h>
#include <vulkan/vulkan_core.h>

#include "utils/types.h"

typedef struct lbr_window_t LbrWindow;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <windows.h>

struct lbr_window_t {
  HWND window_handle;
  MSG message;
  VkExtent2D resolution;
};

#else
#endif

typedef struct lbr_window_create_info_t {
  u32 height;
  u32 width;
  bool resizeable;
  bool show_console;
  const char* name;
} LbrWindowCreateInfo;

void lbrDefineWindow(LbrWindowCreateInfo* p_info, LbrWindow* p_window);
const char* lbrWindowGetVulkanExtensions();
void lbrWindowPollEvents(LbrWindow* p_window);
void lbrWindowDefineVulkanSurface(LbrWindow* p_window, VkInstance instance, VkSurfaceKHR* p_surface);
