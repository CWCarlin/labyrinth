#pragma once

#include "utils/types.h"

typedef struct lbr_register_context_t LbrRegisterContext;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

struct lbr_register_context_t {
  uintptr* rip;  // instruction pointer
  uintptr* rsp;  // stack pointer
  uintptr* gs;   // TLS pointer

  // general purpose x86_64 registers in Windows ABI
  uintptr *r12, *r13, *r14, *r15;
  uintptr *rdi, *rsi, *rbx, *rbp;

  // SIMD registers are callee saved under Windows ABI
  u128 xmm6, xmm7, xmm8, xmm9, xmm10;
  u128 xmm11, xmm12, xmm13, xmm14, xmm15;

  // Windows ABI argument registers
  uintptr *rcx, *rdx;
};

#else

struct lbr_register_context_t {
  uintptr* rip;  // instruction pointer
  uintptr* rsp;  // stack pointer
  uintptr* fs;   // TLS pointer

  // general purpose x86_64 registers in SysV ABI
  uintptr *r12, *r13, *r14, *r15;
  uintptr *rbx, *rbp;

  // SysV ABI argument registers
  uintptr *rdi, *rsi;
};

#endif

typedef struct lbr_fiber_t {
  void* stack;
  LbrRegisterContext context;
} LbrFiber;

void lbrRegisterGetContext(LbrRegisterContext* p_context);
void lbrRegisterSetContext(LbrRegisterContext* p_context);
void lbrRegisterSwapContext(LbrRegisterContext* p_context_from, LbrRegisterContext* p_context_to);
