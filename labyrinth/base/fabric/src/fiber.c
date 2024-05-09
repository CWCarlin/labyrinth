#include "fabric/fiber.h"

// https://gist.github.com/JiayinCao/07475d3423952b702d1efc5268b0df4e
// https://stackoverflow.com/questions/71259613/c-fibers-crashing-on-printf

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_get_register_context:        \n\t"

    "movq     (%rsp),     %r8         \n\t"
    "movq     %r8,        0(%rcx)     \n\t"  // rip -> struct
    "leaq     8(%rsp),    %r8         \n\t"
    "movq     %r8,        8(%rcx)     \n\t"  // rsp -> struct
    "movq     %gs,        %r8         \n\t"
    "movq     %r8,        16(%rcx)    \n\t"  // gs -> struct

    "movq     %r12,       24(%rcx)    \n\t"  // r12 -> struct
    "movq     %r13,       32(%rcx)    \n\t"  // r13 -> struct
    "movq     %r14,       40(%rcx)    \n\t"  // r14 -> struct
    "movq     %r15,       48(%rcx)    \n\t"  // r15 -> struct

    "movq     %rdi,       56(%rcx)    \n\t"  // rdi -> struct
    "movq     %rsi,       64(%rcx)    \n\t"  // rsi -> struct
    "movq     %rbx,       72(%rcx)    \n\t"  // rbx -> struct
    "movq     %rbp,       80(%rcx)    \n\t"  // rbp -> struct

    "movups   %xmm6,      96(%rcx)    \n\t"   // xmm6 -> struct
    "movups   %xmm7,      112(%rcx)    \n\t"  // xmm7 -> struct
    "movups   %xmm8,      128(%rcx)   \n\t"   // xmm8 -> struct
    "movups   %xmm9,      144(%rcx)   \n\t"   // xmm9 -> struct
    "movups   %xmm10,     160(%rcx)   \n\t"   // xmm10 -> struct
    "movups   %xmm11,     176(%rcx)   \n\t"   // xmm11 -> struct
    "movups   %xmm12,     192(%rcx)   \n\t"   // xmm12 -> struct
    "movups   %xmm13,     208(%rcx)   \n\t"   // xmm13 -> struct
    "movups   %xmm14,     224(%rcx)   \n\t"   // xmm14 -> struct
    "movups   %xmm15,     240(%rcx)   \n\t"   // xmm15 -> struct

    "ret");

__asm(
    ".text                            \n"
    ".align 4                         \n"
    "lbr_set_register_context:        \n\t"

    "movq     0(%rcx),     %r8        \n\t"  // struct.rip -> temp
    "movq     8(%rcx),     %rsp       \n\t"  // struct.rsp -> rsp

    "movq     16(%rcx),    %r9        \n\t"
    "movq     %r9,         %gs        \n\t"  // struct.gs -> gs

    "movq     24(%rcx),    %r12       \n\t"  // struct.r12 -> r12
    "movq     32(%rcx),    %r13       \n\t"  // struct.r13 -> r13
    "movq     40(%rcx),    %r14       \n\t"  // struct.r14 -> r14
    "movq     48(%rcx),    %r15       \n\t"  // struct.r15 -> r15

    "movq     56(%rcx),    %rdi       \n\t"  // struct.rdi -> rdi
    "movq     64(%rcx),    %rsi       \n\t"  // struct.rsi -> rsi
    "movq     72(%rcx),    %rbx       \n\t"  // struct.rbx -> rbx
    "movq     80(%rcx),    %rbp       \n\t"  // struct.rbp -> rbp

    "movups   96(%rcx),    %xmm6      \n\t"  // struct.xmm6 -> xmm6
    "movups   112(%rcx),   %xmm7      \n\t"  // struct.xmm7 -> xmm7
    "movups   128(%rcx),   %xmm8      \n\t"  // struct.xmm8 -> xmm8
    "movups   144(%rcx),   %xmm9      \n\t"  // struct.xmm9 -> xmm9
    "movups   160(%rcx),   %xmm10     \n\t"  // struct.xmm10 -> xmm10
    "movups   176(%rcx),   %xmm11     \n\t"  // struct.xmm11 -> xmm11
    "movups   192(%rcx),   %xmm12     \n\t"  // struct.xmm12 -> xmm12
    "movups   208(%rcx),   %xmm13     \n\t"  // struct.xmm13 -> xmm13
    "movups   224(%rcx),   %xmm14     \n\t"  // struct.xmm14 -> xmm14
    "movups   240(%rcx),   %xmm15     \n\t"  // struct.xmm15 -> xmm15

    "movq     256(%rcx),   %rdx       \n\t"  // struct.rdx -> rdx
    "movq     264(%rcx),   %rcx       \n\t"  // struct.rcx -> rcx

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
    "movq     %gs,        %r8         \n\t"
    "movq     %r8,        16(%rcx)    \n\t"  // gs -> struct

    "movq     %r12,       24(%rcx)    \n\t"  // r12 -> struct_from
    "movq     %r13,       32(%rcx)    \n\t"  // r13 -> struct_from
    "movq     %r14,       40(%rcx)    \n\t"  // r14 -> struct_from
    "movq     %r15,       48(%rcx)    \n\t"  // r15 -> struct_from

    "movq     %rdi,       56(%rcx)    \n\t"  // rdi -> struct_from
    "movq     %rsi,       64(%rcx)    \n\t"  // rsi -> struct_from
    "movq     %rbx,       72(%rcx)    \n\t"  // rbx -> struct_from
    "movq     %rbp,       80(%rcx)    \n\t"  // rbp -> struct_from

    "movups   %xmm6,      96(%rcx)    \n\t"  // xmm6 -> struct_from
    "movups   %xmm7,      112(%rcx)   \n\t"  // xmm7 -> struct_from
    "movups   %xmm8,      128(%rcx)   \n\t"  // xmm8 -> struct_from
    "movups   %xmm9,      144(%rcx)   \n\t"  // xmm9 -> struct_from
    "movups   %xmm10,     160(%rcx)   \n\t"  // xmm10 -> struct_from
    "movups   %xmm11,     176(%rcx)   \n\t"  // xmm11 -> struct_from
    "movups   %xmm12,     192(%rcx)   \n\t"  // xmm12 -> struct_from
    "movups   %xmm13,     208(%rcx)   \n\t"  // xmm13 -> struct_from
    "movups   %xmm14,     224(%rcx)   \n\t"  // xmm14 -> struct_from
    "movups   %xmm15,     240(%rcx)   \n\t"  // xmm15 -> struct_from

    // set context
    "movq     0(%rdx),    %r8         \n\t"  // struct_to.rip -> temp
    "movq     8(%rdx),    %rsp        \n\t"  // struct_to.rsp -> rsp
    "movq     16(%rcx),   %r9         \n\t"
    "movq     %r9,        %gs         \n\t"  // struct_to.gs -> gs

    "movq     24(%rdx),   %r12        \n\t"  // struct_to.r12 -> r12
    "movq     32(%rdx),   %r13        \n\t"  // struct_to.r13 -> r13
    "movq     40(%rdx),   %r14        \n\t"  // struct_to.r14 -> r14
    "movq     48(%rdx),   %r15        \n\t"  // struct_to.r15 -> r15

    "movq     56(%rdx),   %rdi        \n\t"  // struct_to.rdi -> rdi
    "movq     64(%rdx),   %rsi        \n\t"  // struct_to.rsi -> rsi
    "movq     72(%rdx),   %rbx        \n\t"  // struct_to.rbx -> rbx
    "movq     80(%rdx),   %rbp        \n\t"  // struct_to.rbp -> rbp

    "movups   96(%rdx),   %xmm6       \n\t"  // struct_to.xmm6 -> xmm6
    "movups   112(%rdx),  %xmm7       \n\t"  // struct_to.xmm7 -> xmm7
    "movups   129(%rdx),  %xmm8       \n\t"  // struct_to.xmm8 -> xmm8
    "movups   144(%rdx),  %xmm9       \n\t"  // struct_to.xmm9 -> xmm9
    "movups   160(%rdx),  %xmm10      \n\t"  // struct_to.xmm10 -> xmm10
    "movups   176(%rdx),  %xmm11      \n\t"  // struct_to.xmm11 -> xmm11
    "movups   192(%rdx),  %xmm12      \n\t"  // struct_to.xmm12 -> xmm12
    "movups   208(%rdx),  %xmm13      \n\t"  // struct_to.xmm13 -> xmm13
    "movups   224(%rdx),  %xmm14      \n\t"  // struct_to.xmm14 -> xmm14
    "movups   240(%rdx),  %xmm15      \n\t"  // struct_to.xmm15 -> xmm15

    "movq     256(%rdx),  %rcx        \n\t"  // struct_to.rdx -> rcx
    "movq     264(%rdx),  %rdx        \n\t"  // struct_to.rcx -> rdx

    "jmpq     *%r8                    \n\t");

#elif defined(__linux__)
#endif

extern __attribute__((noinline)) void lbr_get_register_context(LbrRegisterContext* p_context);
extern __attribute__((noinline)) void lbr_set_register_context(LbrRegisterContext* p_context);
extern __attribute__((noinline)) void lbr_swap_register_context(LbrRegisterContext* p_context_from,
                                                                LbrRegisterContext* p_context_to);

void lbrRegisterGetContext(LbrRegisterContext* p_context) { lbr_get_register_context(p_context); }
void lbrRegisterSetContext(LbrRegisterContext* p_context) { lbr_set_register_context(p_context); }
void lbrRegisterSwapContext(LbrRegisterContext* p_context_from, LbrRegisterContext* p_context_to) {
  lbr_swap_register_context(p_context_from, p_context_to);
}
