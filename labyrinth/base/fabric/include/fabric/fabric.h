#pragma once

#include "fabric/fiber.h"
#include "fabric/lock.h"

void lbrInitializeFabric();
void lbrDismantleFabric();
void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks);
void lbrFabricAwaitLock(LbrSpinLock* p_lock);
void lbrFabricReturn();
