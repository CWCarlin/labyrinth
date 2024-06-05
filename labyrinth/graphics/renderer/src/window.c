#include "renderer/window.h"

#include <errhandlingapi.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <stdio.h>
#include <wincon.h>

#include "utils/logging.h"
#include "vulkan/vulkan_core.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <vulkan/vulkan_win32.h>
#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0;
}

void lbrDefineWindow(LbrWindowCreateInfo* p_info, LbrWindow* p_window) {
  WNDCLASS wc      = {0};
  wc.lpfnWndProc   = WindowProc;
  wc.hInstance     = GetModuleHandle(NULL);
  wc.lpszClassName = "LabyrinthWindowClass";

  LBR_ASSERT(RegisterClass(&wc));

  DWORD style = WS_OVERLAPPEDWINDOW;

  if (!p_info->resizeable) {
    style = style & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
  }

  p_window->window_handle = CreateWindowEx(0, "LabyrinthWindowClass", p_info->name, style, CW_USEDEFAULT, CW_USEDEFAULT, (int)p_info->width, (int)p_info->height, NULL, NULL,
                                           GetModuleHandleA(NULL), NULL);

  LBR_ASSERT((p_window->window_handle));

  ShowWindow(p_window->window_handle, SW_SHOW);
  UpdateWindow(p_window->window_handle);

  RECT rect;
  GetClientRect(p_window->window_handle, &rect);

  p_window->resolution.height = rect.bottom;
  p_window->resolution.width  = rect.right;

  if (p_info->show_console) {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
    SetConsoleTitle(p_info->name);
  }
}

const char* lbrWindowGetVulkanExtensions() { return VK_KHR_WIN32_SURFACE_EXTENSION_NAME; }

void lbrWindowPollEvents(LbrWindow* p_window) {
  while (PeekMessage(&p_window->message, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&p_window->message);
    DispatchMessage(&p_window->message);
  };
}

void lbrWindowDefineVulkanSurface(LbrWindow* p_window, VkInstance instance, VkSurfaceKHR* p_surface) {
  VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
  surface_create_info.sType                       = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  surface_create_info.hinstance                   = GetModuleHandleA(NULL);
  surface_create_info.hwnd                        = p_window->window_handle;

  LBR_ASSERT(vkCreateWin32SurfaceKHR(instance, &surface_create_info, NULL, p_surface) == VK_SUCCESS);
}

#else
#endif
