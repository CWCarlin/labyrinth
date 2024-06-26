#include "fabric/fabric.h"

#include "allocators/linear_alloc.h"
#include "data_structures/list.h"
#include "data_structures/queue.h"
#include "fabric/fiber.h"
#include "fabric/synchronize.h"
#include "fabric/thread.h"
#include "utils/types.h"

#define NUM_FIBERS          200
#define FIBER_STACK_SIZE    16384
#define ALLOCATOR_ALIGNMENT 16
#define MAX_TASKS           2000

#define FABRIC_ALLOCATOR_BYTES(thread_count)                                                                                     \
  ((usize)FIBER_STACK_SIZE * (usize)NUM_FIBERS + thread_count * (sizeof(LbrThread) + sizeof(LbrFiber*) + sizeof(LbrFiber)) +     \
   (usize)NUM_FIBERS * sizeof(LbrFiber) + (usize)NUM_FIBERS * (sizeof(LbrLockedFiber) + sizeof(LbrListNode)) +                   \
   2 * (usize)NUM_FIBERS * sizeof(LbrFiber*) + (usize)MAX_TASKS * sizeof(LbrTask) +                                              \
   (usize)NUM_FIBERS * (sizeof(LbrSemaphore) + sizeof(LbrSemaphore*)))

typedef struct {
  LbrFiber* p_fiber;
  LbrSemaphore* p_semaphore;
  u8 wait_on_semaphore_count;
} LbrLockedFiber;

typedef struct {
  usize num_threads;
  LbrLinearAllocator allocator;

  LbrFiber* fiber_pool;
  LbrThread* thread_pool;
  LbrSemaphore* semaphore_pool;
  LbrFiber* thread_fibers;
  LbrFiber** active_fibers;

  LbrList locked_fibers_list;
  LbrQueue open_fibers_queue;
  LbrQueue unlocked_fibers_queue;
  LbrQueue task_queue;
  LbrQueue semaphore_queue;

  LbrSpinLock locked_fibers_lock;
  LbrSpinLock open_fibers_lock;
  LbrSpinLock unlocked_fibers_lock;
  LbrSpinLock task_lock;
  LbrSpinLock semaphore_lock;
} LbrFabric;

volatile static __thread usize thread_id;
static LbrFabric g_fabric;

static void lbrFabricThreadInit(const usize* id);
static void lbrFabricAwaitTasks();
static void lbrFabricFiberRuntime(LbrTask* task);
static void lbrFabricResetThreadFiber();
static void lbrFabricFreeActiveFiber();

// we initialize the fabric subsystem here.
// after this function exits, all available
// system threads will spin up and await work.
void lbrInitializeFabric() {
  g_fabric.num_threads = lbrThreadGetThreadCount();

  LbrLinearAllocatorCreateInfo alloc_info;
  alloc_info.alignment = ALLOCATOR_ALIGNMENT;
  alloc_info.bytes     = FABRIC_ALLOCATOR_BYTES(g_fabric.num_threads);
  lbrCreateLinearAllocator(&alloc_info, &g_fabric.allocator);

  g_fabric.fiber_pool     = lbrLinearAllocatorAllocate(&g_fabric.allocator, NUM_FIBERS * sizeof(LbrFiber));
  g_fabric.thread_pool    = lbrLinearAllocatorAllocate(&g_fabric.allocator, g_fabric.num_threads * sizeof(LbrThread));
  g_fabric.semaphore_pool = lbrLinearAllocatorAllocate(&g_fabric.allocator, NUM_FIBERS * sizeof(LbrSemaphore));
  g_fabric.thread_fibers  = lbrLinearAllocatorAllocate(&g_fabric.allocator, g_fabric.num_threads * sizeof(LbrFiber));
  g_fabric.active_fibers  = lbrLinearAllocatorAllocate(&g_fabric.allocator, g_fabric.num_threads * sizeof(LbrFiber*));

  LbrListCreateInfo locked_fibers_list_info;
  locked_fibers_list_info.alloc_callbacks = g_fabric.allocator.lbr_callbacks;
  locked_fibers_list_info.type_size       = sizeof(LbrLockedFiber);
  locked_fibers_list_info.capacity        = NUM_FIBERS;
  lbrDefineList(&locked_fibers_list_info, &g_fabric.locked_fibers_list);

  LbrQueueCreateInfo fiber_queue_create_info;
  fiber_queue_create_info.alloc_callbacks = g_fabric.allocator.lbr_callbacks;
  fiber_queue_create_info.type_size       = sizeof(LbrFiber*);
  fiber_queue_create_info.capacity        = NUM_FIBERS;
  lbrDefineQueue(&fiber_queue_create_info, &g_fabric.open_fibers_queue);
  lbrDefineQueue(&fiber_queue_create_info, &g_fabric.unlocked_fibers_queue);

  LbrQueueCreateInfo task_queue_create_info;
  task_queue_create_info.alloc_callbacks = g_fabric.allocator.lbr_callbacks;
  task_queue_create_info.type_size       = sizeof(LbrTask);
  task_queue_create_info.capacity        = MAX_TASKS;
  lbrDefineQueue(&task_queue_create_info, &g_fabric.task_queue);

  LbrQueueCreateInfo semaphore_queue_create_info;
  semaphore_queue_create_info.alloc_callbacks = g_fabric.allocator.lbr_callbacks;
  semaphore_queue_create_info.type_size       = sizeof(LbrSemaphore*);
  semaphore_queue_create_info.capacity        = NUM_FIBERS;
  lbrDefineQueue(&semaphore_queue_create_info, &g_fabric.semaphore_queue);

  lbrDefineSpinLock(&g_fabric.locked_fibers_lock);
  lbrDefineSpinLock(&g_fabric.open_fibers_lock);
  lbrDefineSpinLock(&g_fabric.unlocked_fibers_lock);
  lbrDefineSpinLock(&g_fabric.task_lock);
  lbrDefineSpinLock(&g_fabric.semaphore_lock);

  LbrFiberCreateInfo fiber_create_info;
  fiber_create_info.alloc_callbacks = g_fabric.allocator.lbr_callbacks;
  fiber_create_info.stack_size      = FIBER_STACK_SIZE;
  for (usize i = 0; i < NUM_FIBERS; i++) {
    fiber_create_info.id = i;
    lbrDefineFiber(&fiber_create_info, &g_fabric.fiber_pool[i]);
    LbrFiber* p_fiber = &g_fabric.fiber_pool[i];
    lbrQueuePush(&g_fabric.open_fibers_queue, &p_fiber);

    LbrSemaphore* p_sem = &g_fabric.semaphore_pool[i];
    lbrQueuePush(&g_fabric.semaphore_queue, &p_sem);
  }

  for (usize i = 1; i < g_fabric.num_threads; i++) {
    g_fabric.thread_pool[i].thread_id = i;
    lbrDefineThread((uintptr)lbrFabricThreadInit, &g_fabric.thread_pool[i]);
    lbrThreadSetAffinity(&g_fabric.thread_pool[i], i);
  }

  thread_id = 0;

  g_fabric.thread_pool[thread_id] = lbrThreadGetMainThread();
  // g_fabric.active_fibers[thread_id] = &g_fabric.thread_fibers[thread_id];
  lbrThreadSetAffinity(&g_fabric.thread_pool[thread_id], 0);
  lbrFiberConvertThread(&g_fabric.thread_fibers[thread_id]);
}

// tears down the fabric subsystem for a clean program exit.
void lbrDismantleFabric() {}

LbrTask lbrFabricDefineTask(PFN_lbrTaskFunction pfn_func, void* arg) {
  LbrTask task;
  task.pfn_task = pfn_func;
  task.p_input  = arg;

  return task;
}

// queues up the requested tasks while also returning an awaitable
// fabric semaphore in pp_semaphore if requested.
void lbrFabricQueueTasks(LbrTask* p_tasks, usize num_tasks, LbrSemaphore** pp_semaphore) {
  if (pp_semaphore) {
    lbrSpinLockAcquire(&g_fabric.semaphore_lock);
    lbrQueuePop(&g_fabric.semaphore_queue, pp_semaphore);
    lbrSpinLockRelease(&g_fabric.semaphore_lock);
    lbrDefineSemaphore(num_tasks, *pp_semaphore);
  }

  lbrSpinLockAcquire(&g_fabric.task_lock);
  for (usize i = 0; i < num_tasks; i++) {
    if (pp_semaphore) {
      p_tasks[i].p_semaphore = *pp_semaphore;
    } else {
      p_tasks[i].p_semaphore = NULL;
    }
    lbrQueuePush(&g_fabric.task_queue, &p_tasks[i]);
  }
  lbrSpinLockRelease(&g_fabric.task_lock);
}

// when called from a running task, we store the required information,
// i.e. what fiber it came from, what semaphore it's waiting on, and at
// what point the fiber should unlock. It puts this information into a
// dense linked list and sends the thread back out to grab something new.
// this function is really the entire reason why the fabric system exists.
// this allows for any function to pause execution half way through, and return
// once the condition is met.
void lbrFabricAwaitSemaphore(LbrSemaphore* p_semaphore, u8 count) {
  LbrLockedFiber locked_fiber;
  locked_fiber.p_fiber                 = g_fabric.active_fibers[thread_id];
  locked_fiber.p_semaphore             = p_semaphore;
  locked_fiber.wait_on_semaphore_count = count;

  lbrSpinLockAcquire(&g_fabric.locked_fibers_lock);
  lbrListPushBack(&g_fabric.locked_fibers_list, &locked_fiber);
  lbrSpinLockRelease(&g_fabric.locked_fibers_lock);

  lbrFabricResetThreadFiber();
  lbrFiberSwapContext(g_fabric.active_fibers[thread_id], &g_fabric.thread_fibers[thread_id]);
}

// this just takes a semaphore and tells the fabric subsystem that it
// can have it back to delegate to other awaited fibers.
void lbrFabricFreeSemaphore(LbrSemaphore** pp_semaphore) {
  lbrSpinLockAcquire(&g_fabric.semaphore_lock);
  lbrQueuePush(&g_fabric.semaphore_queue, pp_semaphore);
  lbrSpinLockRelease(&g_fabric.semaphore_lock);
  *pp_semaphore = NULL;
}

// just initializes a threads required information for working in the fabric system.
// the only thread that should go through this is the main thread.
static void lbrFabricThreadInit(const usize* id) {
  thread_id = *id;
  lbrFiberConvertThread(&g_fabric.thread_fibers[*id]);
  lbrFabricAwaitTasks();
}

// this is where threads spend more of their spinning time. They should
// always be looking for a new task to grab and execute.
static void lbrFabricAwaitTasks() {
  LbrTask task;
  task.pfn_task     = NULL;
  LbrFiber* p_fiber = NULL;

  // every queue check is wrapped in an if length > 0
  // to avoid unneeded stalling.
  // We have to recheck length after acquiring since
  // the final elements might have been stolen while
  // waiting to acquire.
  for (;;) {
    // clear out the unlocked fibers queue before moving on.
    // unlocked fibers should be top priority over unstarted fibers.
    if (g_fabric.unlocked_fibers_queue.length > 0) {
      lbrSpinLockAcquire(&g_fabric.unlocked_fibers_lock);
      if (g_fabric.unlocked_fibers_queue.length > 0) {
        lbrQueuePop(&g_fabric.unlocked_fibers_queue, &p_fiber);
      }
      lbrSpinLockRelease(&g_fabric.unlocked_fibers_lock);

      if (p_fiber) {
        g_fabric.active_fibers[thread_id] = p_fiber;
        lbrFiberSetContext(p_fiber);
      }
    }

    // clear out the task queue.
    if (g_fabric.task_queue.length > 0) {
      lbrSpinLockAcquire(&g_fabric.task_lock);
      if (g_fabric.task_queue.length > 0) {
        lbrQueuePop(&g_fabric.task_queue, &task);
      }
      lbrSpinLockRelease(&g_fabric.task_lock);

      if (task.pfn_task) {
        lbrSpinLockAcquire(&g_fabric.open_fibers_lock);
        lbrQueuePop(&g_fabric.open_fibers_queue, &p_fiber);
        lbrSpinLockRelease(&g_fabric.open_fibers_lock);
        lbrFiberPrepare(p_fiber, (uintptr)lbrFabricFiberRuntime, (uintptr)&task);
        g_fabric.active_fibers[thread_id] = p_fiber;
        lbrFiberSwapContext(&g_fabric.thread_fibers[thread_id], p_fiber);
      }
    }

    // if nothing found, we pause and wait for a few cycles
    __builtin_ia32_pause();
  }
}

// this acts a shell for a fiber to run in. This allows
// for any function to be passed as a task function without
// having to worry about interacting with the actual
// fabric subsystem.
// Once a task is completed, if it has an associated semaphore,
// it will decrement that semaphore before freeing the fiber/task
// it was attached to.
// Once all is said and done, it sends the thread back to spin and look
// for more work.
static void lbrFabricFiberRuntime(LbrTask* task) {
  LbrSemaphore* p_semaphore = task->p_semaphore;
  task->pfn_task(task->p_input);

  if (task->p_semaphore) {
    lbrSemaphoreDecrement(p_semaphore);
    lbrSpinLockAcquire(&g_fabric.locked_fibers_lock);
    for (usize i = 0; i < g_fabric.locked_fibers_list.length; i++) {
      LbrLockedFiber* p_locked_fiber = lbrListAt(&g_fabric.locked_fibers_list, i);
      if (p_locked_fiber->p_semaphore->count <= p_locked_fiber->wait_on_semaphore_count) {
        lbrSpinLockAcquire(&g_fabric.unlocked_fibers_lock);
        lbrQueuePush(&g_fabric.unlocked_fibers_queue, &p_locked_fiber->p_fiber);
        lbrListRemove(&g_fabric.locked_fibers_list, p_locked_fiber);
        lbrSpinLockRelease(&g_fabric.unlocked_fibers_lock);
      }
    }
    lbrSpinLockRelease(&g_fabric.locked_fibers_lock);
  }

  lbrFabricFreeActiveFiber();
  lbrFabricResetThreadFiber();
  lbrFiberSetContext(&g_fabric.thread_fibers[thread_id]);
}

// resets the thread's fiber shell to avoid stack overflow.
static void lbrFabricResetThreadFiber() {
  g_fabric.thread_fibers[thread_id].context.rip = (uintptr)lbrFabricAwaitTasks;
  lbrFiberReset(&g_fabric.thread_fibers[thread_id]);
}

// frees the current thread's active fiber by resetting the stack,
// putting it into the free fiber queue, and then setting the current
// active fiber to a NULL handle.
static void lbrFabricFreeActiveFiber() {
  lbrFiberReset(g_fabric.active_fibers[thread_id]);
  lbrSpinLockAcquire(&g_fabric.open_fibers_lock);
  lbrQueuePush(&g_fabric.open_fibers_queue, &g_fabric.active_fibers[thread_id]);
  lbrSpinLockRelease(&g_fabric.open_fibers_lock);
  g_fabric.active_fibers[thread_id] = NULL;
}
