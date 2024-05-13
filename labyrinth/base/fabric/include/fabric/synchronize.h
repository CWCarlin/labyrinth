#pragma once

#include "utils/types.h"

typedef struct lbr_spin_lock_t {
  volatile lbr_bool acquired;
} LbrSpinLock;

typedef struct lbr_semaphore_t {
  volatile u8 count;
} LbrSemaphore;

void lbrCreateSpinLock(LbrSpinLock* p_lock);
lbr_bool lbrSpinLockIsFree(volatile LbrSpinLock* p_lock);
lbr_bool lbrSpinLockTryAcquire(volatile LbrSpinLock* p_lock);
void lbrSpinLockAcquire(volatile LbrSpinLock* p_lock);
void lbrSpinLockRelease(volatile LbrSpinLock* p_lock);

void lbrCreateSemaphore(u8 count, LbrSemaphore* p_semaphore);
void lbrSemaphoreIncrement(volatile LbrSemaphore* p_semaphore);
void lbrSemaphoreDecrement(volatile LbrSemaphore* p_semaphore);
u8 lbrSemaphoreGetCount(volatile LbrSemaphore* p_semaphore);
