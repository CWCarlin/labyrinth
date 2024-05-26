#pragma once

#include "fabric/synchronize.h"

typedef void (*PFN_lbrTaskFunction)(void*);

typedef struct lbr_task_t {
  PFN_lbrTaskFunction pfn_task;
  void* p_input;
  LbrSemaphore* p_semaphore;
} LbrTask;

void lbrInitializeFabric();
void lbrDismantleFabric();
LbrTask lbrFabricDefineTask(PFN_lbrTaskFunction pfn_func, void* arg);
void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks, LbrSemaphore** pp_semaphore);
void lbrFabricAwaitSemaphore(LbrSemaphore* p_semaphore, u8 count);
void lbrFabricFreeSemaphore(LbrSemaphore** pp_semaphore);

#define LBR_TASK(fn, arg) lbrFabricDefineTask((PFN_lbrTaskFunction)(fn), arg)
