#pragma once

#include "fabric/fiber.h"
#include "fabric/lock.h"

typedef enum lbr_task_priority_e {
  LOW,
  MEDIUM,
  HIGH,
} LbrTaskPriority;

typedef struct lbr_task_t {
  PFN_lbrFiberEntryFunction pfn_entry_point;
  void* p_data_in;
  LbrSpinLock spin_lock;
} LbrTask;

usize lbrFabricGetThreadCount();
void lbrInitializeFabric();
void lbrDismantleFabric();
void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks, LbrTaskPriority priority);
void lbrFabricAwaitLock(LbrSpinLock* p_lock);
void lbrFabricReturn();
