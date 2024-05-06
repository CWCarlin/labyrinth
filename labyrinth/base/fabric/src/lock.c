#include "fabric/lock.h"

u8 lbrSpinLockIsFree(LbrSpinLock* p_lock) { return !__atomic_load_n(&p_lock->acquired, __ATOMIC_RELAXED); }

void lbrSpinLockAcquire(LbrSpinLock* p_lock) {
  for (;;) {
    if (__atomic_exchange_n(&p_lock->acquired, 1, __ATOMIC_ACQUIRE) == 0) {
      return;
    }

    while (__atomic_load_n(&p_lock->acquired, __ATOMIC_RELAXED) != 0) {
      __builtin_ia32_pause();
    }
  }
}

void lbrSpinLockRelease(LbrSpinLock* p_lock) { __atomic_exchange_n(&p_lock->acquired, 0, __ATOMIC_RELEASE); }

u8 lbrSpinLockTryAcquire(LbrSpinLock* p_lock) {
  return (__atomic_load_n(&p_lock->acquired, __ATOMIC_RELAXED) == 0) &&
         (__atomic_exchange_n(&p_lock->acquired, 1, __ATOMIC_ACQUIRE) == 0);
}

void lbrSpinLockIncrement(LbrSpinLock* p_lock) { __atomic_add_fetch(&p_lock->acquired, 1, __ATOMIC_ACQUIRE); }
void lbrSpinLockDecrement(LbrSpinLock* p_lock) { __atomic_add_fetch(&p_lock->acquired, -1, __ATOMIC_ACQUIRE); }
