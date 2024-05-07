#include "fabric/thread.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>

usize lbrThreadGetThreadCount() {
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  return sysinfo.dwNumberOfProcessors;
}

usize lbrThreadGetThreadID() { return GetCurrentThreadId(); }

void lbrCreateThread(PFN_lbrEntryFunction pfn_entry, usize flag, LbrThread* p_thread) {
  DWORD* t_id             = (DWORD*)&p_thread->thread_id;
  p_thread->thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pfn_entry, &flag, 0, t_id);
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
#include <unistd.h>

usize lbrGetThreadCount() { return sysconf(_SC_NPROCESSORS_ONLN); }

#endif
