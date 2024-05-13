#include "fabric/thread.h"

#include <time.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>

usize lbrThreadGetThreadCount() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}

void lbrCreateThread(uintptr pfn_entry, LbrThread* p_thread) {
  p_thread->thread_handle =
      CreateThread(NULL, 16328, (LPTHREAD_START_ROUTINE)pfn_entry, &p_thread->thread_id, 0, NULL);  // NOLINT
}

void lbrDestroyThread(LbrThread* p_thread) { CloseHandle(p_thread->thread_handle); }

void lbrThreadSetAffinity(LbrThread* p_thread, usize thread_idx) {
  DWORD_PTR thread_mask = 1;
  for (usize i = 0; i < thread_idx; i++) {
    thread_mask <<= 1;
  }

  SetThreadAffinityMask(p_thread->thread_handle, thread_mask);
}

#elif defined(__unix__) || defined(__unix) || defined(__linux__)
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

usize lbrThreadGetThreadCount() { return 2; }  // sysconf(_SC_NPROCESSORS_ONLN); }

usize lbrThreadGetThreadID() { return pthread_self(); }

void lbrCreateThread(PFN_lbrEntryFunction pfn_entry, usize flag, LbrThread* p_thread) {
  pthread_create(&p_thread->thread_handle, NULL, (void* (*)(void*))pfn_entry, &flag);
  p_thread->thread_id = (usize)p_thread->thread_handle;
}

void lbrDestroyThread(LbrThread* p_thread) { pthread_cancel(p_thread->thread_handle); }

void lbrThreadSetAffinity(LbrThread* p_thread, usize thread_idx) {
  cpu_set_t cpu_set;
  CPU_ZERO(&cpu_set);
  CPU_SET(thread_idx, &cpu_set);
  pthread_setaffinity_np(p_thread->thread_handle, sizeof(cpu_set_t), &cpu_set);
}

#endif
