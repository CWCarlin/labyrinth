#pragma once

#include <signal.h>

#include "utils/types.h"


typedef struct lbr_spin_lock_t {
  sig_atomic_t acquired;
} LbrSpinLock;

u8 lbrSpinLockIsFree(LbrSpinLock* p_lock);
void lbrSpinLockAcquire(LbrSpinLock* p_lock);
void lbrSpinLockRelease(LbrSpinLock* p_lock);
u8 lbrSpinLockTryAcquire(LbrSpinLock* p_lock);
void lbrSpinLockIncrement(LbrSpinLock* p_lock);
void lbrSpinLockDecrement(LbrSpinLock* p_lock);
