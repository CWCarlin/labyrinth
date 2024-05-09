#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"

typedef struct lbr_register_context_t LbrRegisterContext;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

// https://learn.microsoft.com/en-us/cpp/build/x64-software-conventions?view=msvc-170
// https://en.wikipedia.org/wiki/Win32_Thread_Information_Block
struct lbr_register_context_t {
  uintptr rip;  // instruction pointer
  uintptr rsp;  // stack pointer

  // general purpose x86_64 registers in Windows ABI
  uintptr r12, r13, r14, r15;
  uintptr rdi, rsi, rbx, rbp;

  // SIMD registers are callee saved under Windows ABI
  u128 xmm6, xmm7, xmm8, xmm9, xmm10;
  u128 xmm11, xmm12, xmm13, xmm14, xmm15;

  // Windows ABI argument registers
  uintptr rcx, rdx;
};

#else

// file:///C:/Users/Chandler/Downloads/abi.pdf
struct lbr_register_context_t {
  uintptr* rip;  // instruction pointer
  uintptr* rsp;  // stack pointer

  // general purpose x86_64 registers in SysV ABI
  uintptr *r12, *r13, *r14, *r15;
  uintptr *rbx, *rbp;

  // SysV ABI argument registers
  uintptr *rdi, *rsi;
};

#endif

typedef struct lbr_fiber_create_info_t {
  usize id;
  usize stack_size;
  LbrAllocCallbacks alloc_callbacks;
} LbrFiberCreateInfo;

typedef struct lbr_fiber_t {
  usize id;
  void* stack;
  usize stack_size;
  LbrRegisterContext context;
  LbrAllocCallbacks alloc_callbacks;
  u8 from_thread;
} LbrFiber;

typedef struct lbr_task_t {
  PFN_lbrEntryFunction entry_point;
  void* first_argument;
  void* second_argument;
} LbrTask;

void lbrCreateFiber(LbrFiberCreateInfo* p_info, LbrFiber* p_fiber);
void lbrDestroyFiber(LbrFiber* p_fiber);
void lbrFiberConvertThread(LbrFiber* p_fiber);
void lbrFiberGetContext(LbrFiber* p_fiber);
void lbrFiberSetContext(LbrFiber* p_fiber);
void lbrFiberSwapContext(LbrFiber* p_fiber_from, LbrFiber* p_fiber_to);
void lbrFiberSetToTask(LbrFiber* p_fiber, LbrTask* p_task);
void lbrFiberReset(LbrFiber* p_fiber);
