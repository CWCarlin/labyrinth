#include "fabric/synchronize.h"

#include "utils/types.h"

void lbrDefineSpinLock(LbrSpinLock* p_lock) { lbrSpinLockRelease(p_lock); }

bool lbrSpinLockIsFree(volatile LbrSpinLock* p_lock) { return !__atomic_load_n(&p_lock->acquired, __ATOMIC_RELAXED); }

bool lbrSpinLockTryAcquire(volatile LbrSpinLock* p_lock) { return (lbrSpinLockIsFree(p_lock)) && (__atomic_exchange_n(&p_lock->acquired, true, __ATOMIC_ACQUIRE) == false); }

void lbrSpinLockAcquire(volatile LbrSpinLock* p_lock) {
  for (;;) {
    if (lbrSpinLockTryAcquire(p_lock)) {
      return;
    }

    while (!lbrSpinLockIsFree(p_lock)) {
      __builtin_ia32_pause();
    }
  }
}

void lbrSpinLockRelease(volatile LbrSpinLock* p_lock) { __atomic_store_n(&p_lock->acquired, false, __ATOMIC_RELEASE); }

void lbrDefineSemaphore(u8 count, LbrSemaphore* p_semaphore) { __atomic_store_n(&p_semaphore->count, count, __ATOMIC_RELEASE); }
void lbrSemaphoreIncrement(volatile LbrSemaphore* p_semaphore) { __atomic_add_fetch(&p_semaphore->count, 1, __ATOMIC_ACQ_REL); }
void lbrSemaphoreDecrement(volatile LbrSemaphore* p_semaphore) { __atomic_sub_fetch(&p_semaphore->count, 1, __ATOMIC_ACQ_REL); }
u8 lbrSemaphoreGetCount(volatile LbrSemaphore* p_semaphore) { return __atomic_load_n(&p_semaphore->count, __ATOMIC_RELAXED); }
