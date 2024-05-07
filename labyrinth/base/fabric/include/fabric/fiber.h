#pragma once

#include "allocators/alloc_types.h"
#include "utils/types.h"

typedef struct lbr_task_t {
  PFN_lbrEntryFunction p_entry_point;
  void* p_first_arg;
  void* p_second_arg;
  void* p_third_arg;
  void* p_fourth_arg;
} LbrTask;

typedef struct lbr_fiber_context_t LbrFiberContext;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

struct lbr_fiber_context_t {
  void* rip;  // instruction pointer
  void* rsp;  // stack pointer
  void* rbp;  // stack frame pointer

  // input argument
  void *rcx, *rdx, *r8, *r9;

  // general use x64 registers
  void *r12, *r13, *r14, *r15;
  void *rdi, *rsi, *rbx, *_padding_;

  // general use SIMD registers
  u128 xmm6, xmm7, xmm8, xmm9, xmm10;
  u128 xmm11, xmm12, xmm13, xmm14, xmm15;
};

#elif defined(__unix__) || defined(__unix) || defined(__linux__)

struct lbr_fiber_context_t {
  void* rip;  // instruction pointer
  void* rsp;  // stack pointer
  void* rbp;  // stack frame pointer

  // input arguments
  void *rdi, *rsi, *rdx, *rcx;

  // general use x64 registers
  void *r12, *r13, *r14, *r15, *rbx;
};

#endif

typedef struct lbr_fiber_t {
  usize fiber_id;
  void* stack;
  usize stack_size;
  LbrFiberContext context;
  LbrAllocCallbacks alloc_callbacks;
} LbrFiber;

typedef struct lbr_fiber_create_info_t {
  usize fiber_id;
  usize stack_size;
  PFN_lbrEntryFunction pfn_fiber_entry;
  LbrAllocCallbacks alloc_callbacks;
} LbrFiberCreateInfo;

void lbrCreateFiber(LbrFiberCreateInfo* p_info, LbrFiber* p_fiber);
void lbrFiberConvertThread(LbrFiber* p_fiber);
void lbrDestroyFiber(LbrFiber* p_fiber);
void lbrFiberUpdate(LbrFiber* p_fiber);
void lbrFiberSwap(LbrFiber* p_fiber_from, LbrFiber* p_fiber_to);
void lbrFiberSetToTask(LbrFiber* p_fiber, LbrTask* p_task);
