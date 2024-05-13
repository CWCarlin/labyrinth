#include "fabric/synchronize.h"

#include "utils/types.h"

void lbrCreateSpinLock(LbrSpinLock* p_lock) { lbrSpinLockRelease(p_lock); }

lbr_bool lbrSpinLockIsFree(volatile LbrSpinLock* p_lock) { return !__atomic_load_n(&p_lock->acquired, __ATOMIC_RELAXED); }

lbr_bool lbrSpinLockTryAcquire(volatile LbrSpinLock* p_lock) {
  return (lbrSpinLockIsFree(p_lock)) && (__atomic_exchange_n(&p_lock->acquired, LBR_TRUE, __ATOMIC_ACQUIRE) == LBR_FALSE);
}

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

void lbrSpinLockRelease(volatile LbrSpinLock* p_lock) { __atomic_store_n(&p_lock->acquired, LBR_FALSE, __ATOMIC_RELEASE); }

void lbrCreateSemaphore(u8 count, LbrSemaphore* p_semaphore) { __atomic_store_n(&p_semaphore->count, count, __ATOMIC_RELEASE); }
void lbrSemaphoreIncrement(volatile LbrSemaphore* p_semaphore) { __atomic_add_fetch(&p_semaphore->count, 1, __ATOMIC_ACQ_REL); }
void lbrSemaphoreDecrement(volatile LbrSemaphore* p_semaphore) { __atomic_sub_fetch(&p_semaphore->count, 1, __ATOMIC_ACQ_REL); }
u8 lbrSemaphoreGetCount(volatile LbrSemaphore* p_semaphore) { return __atomic_load_n(&p_semaphore->count, __ATOMIC_RELAXED); }
