#include "fabric/fabric.h"

#include <stdio.h>

#include "allocators/block_alloc.h"
#include "allocators/linear_alloc.h"
#include "data_structures/queue.h"
#include "fabric/fiber.h"
#include "fabric/lock.h"
#include "fabric/thread.h"
#include "utils/types.h"

#define NUM_FIBERS        150
#define FIBER_STACK_SIZE  16384
#define FIBER_STACK_ALIGN 16
#define MAX_TASKS         2000

#define LINEAR_ALLOCATOR_BYTES(thread_count)                                                                                     \
  ((usize)MAX_TASKS * sizeof(LbrTask) + (usize)NUM_FIBERS * sizeof(LbrFiber) +                                                   \
   thread_count * (sizeof(LbrFiber) + sizeof(LbrThread)))

LbrBlockAllocator fabric_fiber_stack_allocator;
LbrLinearAllocator fabric_linear_allocator;

LbrThread* fabric_thread_pool;
LbrFiber* fabric_active_fibers;

LbrQueue fabric_open_fibers_queue;
LbrSpinLock fabric_open_fibers_queue_lock;

LbrQueue fabric_task_queue;
LbrSpinLock fabric_task_queue_lock;

LbrSpinLock fabric_thread_setup_lock;

static void lbrFabricFiberAwaitWork(int i) {
  LbrFiber active_fiber = fabric_active_fibers[i];

  for (;;) {
    while (fabric_task_queue.length > 0) {
      if (lbrSpinLockTryAcquire(&fabric_task_queue_lock)) {
        if (fabric_task_queue.length > 0) {
          LbrFiber fiber;
          LbrTask task;
          lbrQueuePop(&fabric_task_queue, &task);
          lbrSpinLockRelease(&fabric_task_queue_lock);

          lbrSpinLockAcquire(&fabric_open_fibers_queue_lock);
          {
            lbrQueuePop(&fabric_open_fibers_queue, &fiber);
            lbrQueuePush(&fabric_open_fibers_queue, &active_fiber);
          }
          lbrSpinLockRelease(&fabric_open_fibers_queue_lock);

          lbrFiberReset(&fiber);
          lbrFiberSetToTask(&fiber, &task);
          fabric_active_fibers[i] = fiber;
          lbrFiberSwapContext(&active_fiber, &fiber);
        }
      }
    }
  }
}

static void lbrFabricThreadSetup(const int* i) {
  LbrFiber fiber;
  lbrFiberConvertThread(&fiber);
  fiber.id                 = *i;
  fabric_active_fibers[*i] = fiber;
  lbrSpinLockRelease(&fabric_thread_setup_lock);
  lbrFabricFiberAwaitWork(*i);
}

void lbrInitializeFabric() {
  usize num_threads = 2;  // lbrThreadGetThreadCount();

  LbrBlockAllocatorCreateInfo block_alloc_info;
  block_alloc_info.num_blocks = NUM_FIBERS;
  block_alloc_info.block_size = FIBER_STACK_SIZE + FIBER_STACK_ALIGN;
  block_alloc_info.alignment  = FIBER_STACK_ALIGN;

  LbrLinearAllocatorCreateInfo linear_alloc_info;
  linear_alloc_info.bytes     = LINEAR_ALLOCATOR_BYTES(num_threads);
  linear_alloc_info.alignment = FIBER_STACK_ALIGN;

  lbrCreateBlockAllocator(&block_alloc_info, &fabric_fiber_stack_allocator);
  lbrCreateLinearAllocator(&linear_alloc_info, &fabric_linear_allocator);

  LbrQueueCreateInfo open_fibers_queue_info;
  open_fibers_queue_info.alloc_callbacks = fabric_linear_allocator.lbr_callbacks;
  open_fibers_queue_info.capacity        = NUM_FIBERS;
  open_fibers_queue_info.type_size       = sizeof(LbrFiber);

  LbrQueueCreateInfo task_queue_info;
  task_queue_info.alloc_callbacks = fabric_linear_allocator.lbr_callbacks;
  task_queue_info.capacity        = MAX_TASKS;
  task_queue_info.type_size       = sizeof(LbrTask);

  lbrCreateQueue(&open_fibers_queue_info, &fabric_open_fibers_queue);
  lbrCreateQueue(&task_queue_info, &fabric_task_queue);

  fabric_active_fibers = lbrLinearAllocatorAllocate(&fabric_linear_allocator, sizeof(LbrFiber) * num_threads);

  LbrFiberCreateInfo fiber_info;
  fiber_info.alloc_callbacks = fabric_fiber_stack_allocator.lbr_callbacks;
  fiber_info.stack_size      = FIBER_STACK_SIZE;

  for (usize i = num_threads; i < NUM_FIBERS; i++) {
    LbrFiber fiber;
    fiber_info.id = i;
    lbrCreateFiber(&fiber_info, &fiber);
    lbrQueuePush(&fabric_open_fibers_queue, &fiber);
  }

  fabric_thread_pool = lbrLinearAllocatorAllocate(&fabric_linear_allocator, sizeof(LbrThread) * num_threads);
  for (usize i = 1; i < num_threads; i++) {
    lbrSpinLockAcquire(&fabric_thread_setup_lock);
    LbrThread* p_thread = &fabric_thread_pool[i];
    lbrCreateThread((PFN_lbrEntryFunction)lbrFabricThreadSetup, i, p_thread);
    lbrThreadSetAffinity(p_thread, i);
  }
}

void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks) {
  lbrSpinLockAcquire(&fabric_task_queue_lock);
  {
    for (usize i = 0; i < num_tasks; i++) {
      lbrQueuePush(&fabric_task_queue, &p_tasks[i]);
    }
  }
  lbrSpinLockRelease(&fabric_task_queue_lock);
}

void lbrFabricReturn() {
  LbrFiber fiber;
  lbrFiberGetContext(&fiber);

  for (int i = 1; i < 2; i++) {
    lbrFabricFiberAwaitWork(i);
  }
}
