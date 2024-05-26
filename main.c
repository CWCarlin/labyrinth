#include <stdio.h>
#include <windows.h>

#include "fabric/fabric.h"
#include "fabric/synchronize.h"
#include "renderer/core.h"
#include "renderer/gpu_allocator.h"
#include "renderer/window.h"
#include "utils/types.h"

#ifdef NDEBUG
#define VALIDATION 0
#else
#define VALIDATION 1
#endif

void foo() { printf("foo\n"); }

void baz(const usize* i) {
  LbrTask new_tasks[10];
  for (usize i = 0; i < 10; i++) {
    new_tasks[i] = LBR_TASK(foo, NULL);
  }

  LbrSemaphore* p_sem = NULL;
  lbrFabricQueueTasks(new_tasks, 10, &p_sem);
  lbrFabricAwaitSemaphore(p_sem, 0);
  lbrFabricFreeSemaphore(&p_sem);
  printf("heee %zu\n", *i);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {  // NOLINT
  (void)hInstance;
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nShowCmd;

  // lbrInitializeFabric();
  // LbrTask tasks[10];
  // usize in[10];
  // for (usize i = 0; i < 10; i++) {
  //   in[i]    = i;
  //   tasks[i] = LBR_TASK(baz, &in[i]);
  // }

  // LbrSemaphore* semm;
  // lbrFabricQueueTasks(tasks, 10, &semm);

  LbrWindowCreateInfo window_info = {0};
  window_info.height              = 720;
  window_info.width               = 1280;
  window_info.resizeable          = LBR_FALSE;
  window_info.show_console        = LBR_TRUE;
  window_info.name                = "Labyrinth";

  LbrWindow window;
  lbrDefineWindow(&window_info, &window);

  LbrVulkanCore core       = lbrCreateVulkanCore(&window, VALIDATION);
  LbrSuitableGPUs gpus     = lbrEnumerateConnectedGPUs(&core);
  LbrLogicalDevice device  = lbrCreateLogicalDevice(&core, &gpus.gpu[0], 3);
  LbrDeviceAllocator alloc = lbrCreateDeviceAllocator(&core, &device);
  (void)alloc;

  for (;;) {
    lbrWindowPollEvents(&window);
  }

  return 0;
}
