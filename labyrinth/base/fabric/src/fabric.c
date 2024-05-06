#include "fabric/fabric.h"

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

#include "allocators/block_alloc.h"
#include "allocators/linear_alloc.h"
#include "data_structures/list.h"
#include "data_structures/queue.h"
#include "fabric/fiber.h"
#include "fabric/lock.h"

#define NUM_FIBERS        100
#define FIBER_STACK_SIZE  16384
#define FIBER_STACK_ALIGN 64

#define LINEAR_ALLOCATOR_PADDING 4096

#define MAX_TASKS 1000

LbrBlockAllocator fiber_block_alloc;
LbrLinearAllocator task_storage_alloc;

pthread_t* fabric_threads;
LbrFiber* running_fibers;

LbrList locked_fibers;
LbrSpinLock locked_fibers_lock = {0};

LbrQueue open_fibers;
LbrSpinLock open_fibers_lock = {0};

LbrQueue low_priority_tasks;
LbrSpinLock low_priority_tasks_lock = {0};

LbrQueue medium_priority_tasks;
LbrSpinLock medium_priority_tasks_lock = {0};

LbrQueue high_priority_tasks;
LbrSpinLock high_priority_tasks_lock = {0};

#ifdef _WIN32
#include <sysinfoapi.h>
usize lbrFabricGetThreadCount() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}
#elif
usize lbrFabricGetThreadCount() { return sysconf(_SC_NPROCESSORS_ONLN); }
#endif

static void lbrFabricFiberRuntime() {
  for (;;) {
    while (high_priority_tasks.length > 0) {
      if (lbrSpinLockTryAcquire(&high_priority_tasks_lock)) {
        if (high_priority_tasks.length > 0) {
          LbrTask task;
          LbrFiber fiber;

          lbrQueuePop(&high_priority_tasks, &task);
          lbrSpinLockRelease(&high_priority_tasks_lock);

          lbrSpinLockAcquire(&open_fibers_lock);
          lbrQueuePop(&open_fibers, &fiber);
          lbrQueuePush(&open_fibers, &running_fibers[pthread_self() - 1]);
          lbrSpinLockRelease(&open_fibers_lock);

          fiber.context.rip = (void*)task.pfn_entry_point;
          fiber.context.rcx = task.p_data_in;
          lbrFiberSwap(&running_fibers[pthread_self() - 1], &fiber);
        }
      }
    }
  }
}

static void* lbrFabricThreadInitialize() {
  LbrFiber* fiber = &running_fibers[pthread_self() - 1];
  lbrFiberConvertThread(fiber);

  lbrFabricFiberRuntime();

  return NULL;
}

void lbrInitializeFabric() {
  LbrBlockAllocatorCreateInfo block_alloc_info;
  block_alloc_info.block_size = FIBER_STACK_SIZE;
  block_alloc_info.num_blocks = NUM_FIBERS;
  block_alloc_info.alignment  = FIBER_STACK_ALIGN;
  lbrCreateBlockAllocator(&block_alloc_info, &fiber_block_alloc);

  usize thread_count = lbrFabricGetThreadCount();

  usize linear_alloc_bytes = NUM_FIBERS * sizeof(LbrFiber) * 3;
  linear_alloc_bytes += thread_count * sizeof(pthread_t);
  linear_alloc_bytes += MAX_TASKS * sizeof(LbrTask) * 3;
  linear_alloc_bytes += LINEAR_ALLOCATOR_PADDING;

  LbrLinearAllocatorCreateInfo linear_alloc_info;
  linear_alloc_info.bytes     = linear_alloc_bytes;
  linear_alloc_info.alignment = FIBER_STACK_ALIGN;
  lbrCreateLinearAllocator(&linear_alloc_info, &task_storage_alloc);

  LbrListCreateInfo list_info;
  list_info.alloc_callbacks = task_storage_alloc.lbr_callbacks;
  list_info.capacity        = NUM_FIBERS;
  list_info.type_size       = sizeof(LbrFiber);

  lbrCreateList(&list_info, &locked_fibers);

  LbrQueueCreateInfo task_queue_info;
  task_queue_info.alloc_callbacks = task_storage_alloc.lbr_callbacks;
  task_queue_info.capacity        = MAX_TASKS;
  task_queue_info.type_size       = sizeof(LbrTask);

  lbrCreateQueue(&task_queue_info, &low_priority_tasks);
  lbrCreateQueue(&task_queue_info, &medium_priority_tasks);
  lbrCreateQueue(&task_queue_info, &high_priority_tasks);

  LbrQueueCreateInfo fiber_queue_info;
  fiber_queue_info.alloc_callbacks = task_storage_alloc.lbr_callbacks;
  fiber_queue_info.capacity        = NUM_FIBERS;
  fiber_queue_info.type_size       = sizeof(LbrFiber);

  lbrCreateQueue(&fiber_queue_info, &open_fibers);

  LbrFiberCreateInfo fiber_info;
  fiber_info.alloc_callbacks = fiber_block_alloc.lbr_callbacks;
  fiber_info.stack_size      = FIBER_STACK_SIZE;

  for (usize i = 0; i < NUM_FIBERS; i++) {
    LbrFiber f;
    lbrCreateFiber(&fiber_info, &f);
    lbrQueuePush(&open_fibers, &f);
  }

  running_fibers = lbrLinearAllocatorAllocate(&task_storage_alloc, sizeof(LbrFiber) * thread_count);
  fabric_threads = lbrLinearAllocatorAllocate(&task_storage_alloc, sizeof(pthread_t) * thread_count);
  for (usize i = 1; i < thread_count; i++) {
    pthread_create(&fabric_threads[i], NULL, lbrFabricThreadInitialize, NULL);
  }
}

void lbrDismantleFabric() {
  free(fiber_block_alloc.data);
  free(task_storage_alloc.data);
}

void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks, LbrTaskPriority priority) {
  LbrQueue* task_queue;
  LbrSpinLock* queue_lock;
  switch (priority) {
    case LOW:
      task_queue = &low_priority_tasks;
      queue_lock = &low_priority_tasks_lock;
      break;
    case MEDIUM:
      task_queue = &medium_priority_tasks;
      queue_lock = &medium_priority_tasks_lock;
      break;
    case HIGH:
      task_queue = &high_priority_tasks;
      queue_lock = &high_priority_tasks_lock;
      break;
  }

  lbrSpinLockAcquire(queue_lock);
  for (usize i = 0; i < num_tasks; i++) {
    lbrQueuePush(task_queue, &p_tasks[i]);
  }
  lbrSpinLockRelease(queue_lock);
}

void lbrFabricReturn() {
  lbrSpinLockAcquire(&open_fibers_lock);
  LbrFiber f;
  lbrQueuePop(&open_fibers, &f);
  lbrSpinLockRelease(&open_fibers_lock);
  f.context.rip = (void*)lbrFabricFiberRuntime;
  lbrFiberSwap(&running_fibers[pthread_self() - 1], &f);
}
