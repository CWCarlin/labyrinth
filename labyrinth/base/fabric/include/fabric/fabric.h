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
void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks, LbrSemaphore** pp_semaphore);
void lbrFabricWaitForSemaphore(LbrSemaphore* p_semaphore, u8 count);
void lbrFabricFreeSemaphore(LbrSemaphore** pp_semaphore);
