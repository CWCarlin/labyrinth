#pragma once

#include "fabric/fiber.h"
#include "fabric/lock.h"

usize lbrFabricGetThreadCount();
void lbrInitializeFabric();
void lbrDismantleFabric();
void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks, LbrTaskPriority priority);
void lbrFabricAwaitLock(LbrSpinLock* p_lock);
void lbrFabricReturn();
