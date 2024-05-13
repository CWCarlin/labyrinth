#include "fabric/fiber.h"

#include <string.h>

#include "allocators/alloc_types.h"
#include "utils/types.h"

// https://gist.github.com/JiayinCao/07475d3423952b702d1efc5268b0df4e
// https://stackoverflow.com/questions/71259613/c-fibers-crashing-on-printf

#define LBR_FIBER_STACK_ALIGNMENT   16
#define LBR_FIBER_THREAD_STACK_SIZE 16384

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_get_register_context:        \n\t"

    "movq     (%rsp),     %r8         \n\t"
    "movq     %r8,        0(%rcx)     \n\t"  // rip -> struct
    "leaq     8(%rsp),    %r8         \n\t"
    "movq     %r8,        8(%rcx)     \n\t"  // rsp -> struct

    "movq     %r12,       16(%rcx)    \n\t"  // r12 -> struct
    "movq     %r13,       24(%rcx)    \n\t"  // r13 -> struct
    "movq     %r14,       32(%rcx)    \n\t"  // r14 -> struct
    "movq     %r15,       40(%rcx)    \n\t"  // r15 -> struct

    "movq     %rdi,       48(%rcx)    \n\t"  // rdi -> struct
    "movq     %rsi,       56(%rcx)    \n\t"  // rsi -> struct
    "movq     %rbx,       64(%rcx)    \n\t"  // rbx -> struct
    "movq     %rbp,       72(%rcx)    \n\t"  // rbp -> struct

    "movups   %xmm6,      80(%rcx)    \n\t"  // xmm6 -> struct
    "movups   %xmm7,      96(%rcx)   \n\t"   // xmm7 -> struct
    "movups   %xmm8,      112(%rcx)   \n\t"  // xmm8 -> struct
    "movups   %xmm9,      128(%rcx)   \n\t"  // xmm9 -> struct
    "movups   %xmm10,     144(%rcx)   \n\t"  // xmm10 -> struct
    "movups   %xmm11,     160(%rcx)   \n\t"  // xmm11 -> struct
    "movups   %xmm12,     176(%rcx)   \n\t"  // xmm12 -> struct
    "movups   %xmm13,     192(%rcx)   \n\t"  // xmm13 -> struct
    "movups   %xmm14,     208(%rcx)   \n\t"  // xmm14 -> struct
    "movups   %xmm15,     224(%rcx)   \n\t"  // xmm15 -> struct

    "movq     %gs,        %r8         \n\t"
    "movq     %r8,        256(%rcx)   \n\t"

    "ret");

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_set_register_context:        \n\t"

    "movq     0(%rcx),     %r8        \n\t"  // struct.rip -> temp
    "movq     8(%rcx),     %rsp       \n\t"  // struct.rsp -> rsp

    "movq     16(%rcx),    %r12       \n\t"  // struct.r12 -> r12
    "movq     24(%rcx),    %r13       \n\t"  // struct.r13 -> r13
    "movq     32(%rcx),    %r14       \n\t"  // struct.r14 -> r14
    "movq     40(%rcx),    %r15       \n\t"  // struct.r15 -> r15

    "movq     48(%rcx),    %rdi       \n\t"  // struct.rdi -> rdi
    "movq     56(%rcx),    %rsi       \n\t"  // struct.rsi -> rsi
    "movq     64(%rcx),    %rbx       \n\t"  // struct.rbx -> rbx
    "movq     72(%rcx),    %rbp       \n\t"  // struct.rbp -> rbp

    "movups   80(%rcx),    %xmm6      \n\t"  // struct.xmm6 -> xmm6
    "movups   96(%rcx),    %xmm7      \n\t"  // struct.xmm7 -> xmm7
    "movups   112(%rcx),   %xmm8      \n\t"  // struct.xmm8 -> xmm8
    "movups   128(%rcx),   %xmm9      \n\t"  // struct.xmm9 -> xmm9
    "movups   144(%rcx),   %xmm10     \n\t"  // struct.xmm10 -> xmm10
    "movups   160(%rcx),   %xmm11     \n\t"  // struct.xmm11 -> xmm11
    "movups   176(%rcx),   %xmm12     \n\t"  // struct.xmm12 -> xmm12
    "movups   192(%rcx),   %xmm13     \n\t"  // struct.xmm13 -> xmm13
    "movups   208(%rcx),   %xmm14     \n\t"  // struct.xmm14 -> xmm14
    "movups   224(%rcx),   %xmm15     \n\t"  // struct.xmm15 -> xmm15

    "movq     240(%rcx),   %rdx       \n\t"  // struct.rdx -> rdx
    "movq     256(%rcx),   %r9        \n\t"
    "movq     %r9,         %gs        \n\t"
    "movq     248(%rcx),   %rcx       \n\t"  // struct.rcx -> rcx

    "jmpq     *%r8                    \n\t");

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_swap_register_context:       \n\t"

    // get context
    "movq     (%rsp),     %r8         \n\t"
    "movq     %r8,        0(%rcx)     \n\t"  // rip -> struct_from
    "leaq     8(%rsp),    %r8         \n\t"
    "movq     %r8,        8(%rcx)     \n\t"  // rsp -> struct_from

    "movq     %r12,       16(%rcx)    \n\t"  // r12 -> struct_from
    "movq     %r13,       24(%rcx)    \n\t"  // r13 -> struct_from
    "movq     %r14,       32(%rcx)    \n\t"  // r14 -> struct_from
    "movq     %r15,       40(%rcx)    \n\t"  // r15 -> struct_from

    "movq     %rdi,       48(%rcx)    \n\t"  // rdi -> struct_from
    "movq     %rsi,       56(%rcx)    \n\t"  // rsi -> struct_from
    "movq     %rbx,       64(%rcx)    \n\t"  // rbx -> struct_from
    "movq     %rbp,       72(%rcx)    \n\t"  // rbp -> struct_from

    "movups   %xmm6,      80(%rcx)    \n\t"  // xmm6 -> struct_from
    "movups   %xmm7,      96(%rcx)    \n\t"  // xmm7 -> struct_from
    "movups   %xmm8,      112(%rcx)   \n\t"  // xmm8 -> struct_from
    "movups   %xmm9,      128(%rcx)   \n\t"  // xmm9 -> struct_from
    "movups   %xmm10,     144(%rcx)   \n\t"  // xmm10 -> struct_from
    "movups   %xmm11,     160(%rcx)   \n\t"  // xmm11 -> struct_from
    "movups   %xmm12,     176(%rcx)   \n\t"  // xmm12 -> struct_from
    "movups   %xmm13,     192(%rcx)   \n\t"  // xmm13 -> struct_from
    "movups   %xmm14,     208(%rcx)   \n\t"  // xmm14 -> struct_from
    "movups   %xmm15,     224(%rcx)   \n\t"  // xmm15 -> struct_from

    "movq     %gs,        %r8         \n\t"
    "movq     %r8,        256(%rcx)   \n\t"

    // set context
    "movq     0(%rdx),     %r8        \n\t"  // struct_tp.rip -> temp
    "movq     8(%rdx),     %rsp       \n\t"  // struct_to.rsp -> rsp

    "movq     16(%rdx),    %r12       \n\t"  // struct_to.r12 -> r12
    "movq     24(%rdx),    %r13       \n\t"  // struct_to.r13 -> r13
    "movq     32(%rdx),    %r14       \n\t"  // struct_to.r14 -> r14
    "movq     40(%rdx),    %r15       \n\t"  // struct_to.r15 -> r15

    "movq     48(%rdx),    %rdi       \n\t"  // struct_to.rdi -> rdi
    "movq     56(%rdx),    %rsi       \n\t"  // struct_to.rsi -> rsi
    "movq     64(%rdx),    %rbx       \n\t"  // struct_to.rbx -> rbx
    "movq     72(%rdx),    %rbp       \n\t"  // struct_to.rbp -> rbp

    "movups   80(%rdx),    %xmm6      \n\t"  // struct_to.xmm6 -> xmm6
    "movups   96(%rdx),    %xmm7      \n\t"  // struct_to.xmm7 -> xmm7
    "movups   112(%rdx),   %xmm8      \n\t"  // struct_to.xmm8 -> xmm8
    "movups   128(%rdx),   %xmm9      \n\t"  // struct_to.xmm9 -> xmm9
    "movups   144(%rdx),   %xmm10     \n\t"  // struct_to.xmm10 -> xmm10
    "movups   160(%rdx),   %xmm11     \n\t"  // struct_to.xmm11 -> xmm11
    "movups   176(%rdx),   %xmm12     \n\t"  // struct_to.xmm12 -> xmm12
    "movups   192(%rdx),   %xmm13     \n\t"  // struct_to.xmm13 -> xmm13
    "movups   208(%rdx),   %xmm14     \n\t"  // struct_to.xmm14 -> xmm14
    "movups   224(%rdx),   %xmm15     \n\t"  // struct_to.xmm15 -> xmm15

    "movq     240(%rdx),   %rcx       \n\t"  // struct_to.rdx -> rcx
    "movq     256(%rdx),   %r9        \n\t"
    "movq     %r9,         %gs        \n\t"
    "movq     248(%rdx),   %rdx       \n\t"  // struct_to.rcx -> rdx

    "jmpq     *%r8                    \n\t");

#elif defined(__linux__)

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_get_register_context:        \n\t"

    "movq     (%rsp),     %r8         \n\t"
    "movq     %r8,        0(%rdi)     \n\t"  // rip -> struct
    "leaq     8(%rsp),    %r8         \n\t"
    "movq     %r8,        8(%rdi)     \n\t"  // rsp -> struct

    "movq     %r12,       16(%rdi)    \n\t"  // r12 -> struct
    "movq     %r13,       24(%rdi)    \n\t"  // r13 -> struct
    "movq     %r14,       32(%rdi)    \n\t"  // r14 -> struct
    "movq     %r15,       40(%rdi)    \n\t"  // r15 -> struct

    "movq     %rbx,       48(%rdi)    \n\t"  // rbx -> struct
    "movq     %rbp,       56(%rdi)    \n\t"  // rbp -> struct

    "movq     %fs,        %r8         \n\t"
    "movq     %r8,        80(%rdi)    \n\t"

    "ret");

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_set_register_context:        \n\t"

    "movq     0(%rdi),     %r8        \n\t"  // struct.rip -> temp
    "movq     8(%rdi),     %rsp       \n\t"  // struct.rsp -> rsp

    "movq     16(%rdi),    %r12       \n\t"  // struct.r12 -> r12
    "movq     24(%rdi),    %r13       \n\t"  // struct.r13 -> r13
    "movq     32(%rdi),    %r14       \n\t"  // struct.r14 -> r14
    "movq     40(%rdi),    %r15       \n\t"  // struct.r15 -> r15

    "movq     48(%rdi),    %rbx       \n\t"  // struct.rbx -> rbx
    "movq     56(%rdi),    %rbp       \n\t"  // struct.rbp -> rbp

    "movq     72(%rdi),    %rsi       \n\t"  // struct.rsi -> rsi
    "movq     80(%rdi),    %r9        \n\t"
    "movq     %r9,         %fs        \n\t"
    "movq     64(%rdi),    %rdi       \n\t"  // struct.rdi -> rdi

    "jmpq     *%r8                    \n\t");

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_swap_register_context:       \n\t"

    // get context
    "movq     (%rsp),     %r8         \n\t"
    "movq     %r8,        0(%rdi)     \n\t"  // rip -> struct
    "leaq     8(%rsp),    %r8         \n\t"
    "movq     %r8,        8(%rdi)     \n\t"  // rsp -> struct

    "movq     %r12,       16(%rdi)    \n\t"  // r12 -> struct
    "movq     %r13,       24(%rdi)    \n\t"  // r13 -> struct
    "movq     %r14,       32(%rdi)    \n\t"  // r14 -> struct
    "movq     %r15,       40(%rdi)    \n\t"  // r15 -> struct

    "movq     %rbx,       48(%rdi)    \n\t"  // rbx -> struct
    "movq     %rbp,       56(%rdi)    \n\t"  // rbp -> struct

    "movq     %fs,        %r8         \n\t"
    "movq     %r8,        80(%rdi)    \n\t"

    // set context
    "movq     0(%rdi),     %r8        \n\t"  // struct.rip -> temp
    "movq     8(%rdi),     %rsp       \n\t"  // struct.rsp -> rsp

    "movq     16(%rdi),    %r12       \n\t"  // struct.r12 -> r12
    "movq     24(%rdi),    %r13       \n\t"  // struct.r13 -> r13
    "movq     32(%rdi),    %r14       \n\t"  // struct.r14 -> r14
    "movq     40(%rdi),    %r15       \n\t"  // struct.r15 -> r15

    "movq     48(%rdi),    %rbx       \n\t"  // struct.rbx -> rbx
    "movq     56(%rdi),    %rbp       \n\t"  // struct.rbp -> rbp

    "movq     72(%rdi),    %rsi       \n\t"  // struct.rsi -> rsi
    "movq     80(%rdi),    %r9        \n\t"
    "movq     %r9,         %fs        \n\t"
    "movq     64(%rdi),    %rdi       \n\t"  // struct.rdi -> rdi

    "jmpq     *%r8                    \n\t");

#endif

extern __attribute__((noinline)) void lbr_get_register_context(volatile LbrRegisterContext* p_context);
extern __attribute__((noinline)) void lbr_set_register_context(volatile LbrRegisterContext* p_context);
extern __attribute__((noinline)) void lbr_swap_register_context(volatile LbrRegisterContext* p_context_from,
                                                                volatile LbrRegisterContext* p_context_to);

void lbrCreateFiber(LbrFiberCreateInfo* p_info, LbrFiber* p_fiber) {
  p_fiber->alloc_callbacks = p_info->alloc_callbacks;
  p_fiber->id              = p_info->id;
  p_fiber->stack           = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, p_info->stack_size);

  uintptr stack_top = (uintptr)((u8*)(p_fiber->stack) + p_info->stack_size) - LBR_FIBER_STACK_ALIGNMENT;
  LBR_ALIGN(stack_top, LBR_FIBER_STACK_ALIGNMENT);
  p_fiber->context.rbp = (uintptr)p_fiber->stack;
  p_fiber->context.rsp = stack_top;
  p_fiber->stack_top   = p_fiber->context.rsp;
}

void lbrDestroyFiber(LbrFiber* p_fiber) {
  lbrAllocCallbacksFree(&p_fiber->alloc_callbacks, p_fiber->stack);
  memset(p_fiber, 0, sizeof(LbrFiber));
}

void lbrFiberConvertThread(LbrFiber* p_fiber) {
  lbr_get_register_context(&p_fiber->context);
  p_fiber->stack_top = p_fiber->context.rsp;
  p_fiber->stack     = (void*)(p_fiber->stack_top);  // NOLINT
}

void lbrFiberGetContext(volatile LbrFiber* p_fiber) { lbr_get_register_context(&p_fiber->context); }

void lbrFiberSetContext(volatile LbrFiber* p_fiber) { lbr_set_register_context(&p_fiber->context); }

void lbrFiberSwapContext(volatile LbrFiber* p_fiber_from, volatile LbrFiber* p_fiber_to) {
  lbr_swap_register_context(&p_fiber_from->context, &p_fiber_to->context);
}

void lbrFiberReset(LbrFiber* p_fiber) { p_fiber->context.rsp = p_fiber->stack_top; }
