#pragma once

#include "utils/types.h"

typedef struct lbr_thread_t LbrThread;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>

struct lbr_thread_t {
  usize thread_id;
  HANDLE thread_handle;
};

#elif defined(__unix__) || defined(__unix) || defined(__linux__)
#include <pthread.h>

struct lbr_thread_t {
  usize thread_id;
  pthread_t thread_handle;
};

#endif

usize lbrThreadGetThreadCount();
usize lbrThreadGetThreadID();
void lbrCreateThread(PFN_lbrEntryFunction pfn_entry, usize flag, LbrThread* p_thread);
void lbrDestroyThread(LbrThread* p_thread);
void lbrThreadSetAffinity(LbrThread* p_thread, usize thread_idx);
