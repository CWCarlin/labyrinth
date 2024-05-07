#include "fabric/fiber.h"

#include "allocators/alloc_types.h"
#include "utils/types.h"
#include "utils/logging.h"

#define STACK_ALIGNMENT 16

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

__asm(
    ".text								        \n"
    ".align 4							        \n"
    "lbr_update_context:				  \n\t"
    "movq   (%rsp),     %rax		  \n\t"
    "movq	  %rax,		    0(%rcx)		\n\t"  // move RIP into context
    "leaq	  8(%rsp),	  %rax		  \n\t"
    "movq	  %rax,		    8(%rcx)		\n\t"  // move RSP into context
    "movq 	%rbp,		    16(%rcx)	\n\t"  // move RBP into context

    // move general purpose x64 registers into context
    "movq	  %r12,		    56(%rcx)	\n\t"
    "movq	  %r13,		    64(%rcx)	\n\t"
    "movq	  %r14,		    72(%rcx)	\n\t"
    "movq	  %r15,		    80(%rcx)	\n\t"
    "movq	  %rdi,		    88(%rcx)	\n\t"
    "movq	  %rsi,		    96(%rcx)	\n\t"
    "movq	  %rbx,		    104(%rcx)	\n\t"

    // move general purpose SIMD registers into context
    "movups	%xmm6,		  128(%rcx)	\n\t"
    "movups	%xmm7,		  144(%rcx)	\n\t"
    "movups	%xmm8,		  160(%rcx)	\n\t"
    "movups	%xmm9,		  176(%rcx)	\n\t"
    "movups	%xmm10,		  192(%rcx)	\n\t"
    "movups	%xmm11,		  208(%rcx)	\n\t"
    "movups	%xmm12,		  224(%rcx)	\n\t"
    "movups	%xmm13,		  240(%rcx)	\n\t"
    "movups	%xmm14,		  256(%rcx)	\n\t"
    "movups	%xmm15,		  272(%rcx)	\n\t"

    "ret");

__asm(
    ".text								       \n"
    ".align 4							       \n"
    "lbr_swap_context:					 \n\t"
    "movq	  (%rsp),		  %rax		 \n\t"
    "movq	  %rax,		    0(%rcx)	 \n\t"  // move RIP into context
    "leaq	  8(%rsp),	  %rax		 \n\t"
    "movq	  %rax,		    8(%rcx)	 \n\t"  // move RSP into context
    "movq 	%rbp,		    16(%rcx) \n\t"  // move RBP into context

    // move general purpose x64 registers into context
    "movq	  %r12,		    56(%rcx) \n\t"
    "movq	  %r13,		    64(%rcx) \n\t"
    "movq	  %r14,		    72(%rcx) \n\t"
    "movq	  %r15,		    80(%rcx) \n\t"
    "movq	  %rdi,		    88(%rcx) \n\t"
    "movq	  %rsi,		    96(%rcx) \n\t"
    "movq	  %rbx,		    104(%rcx)\n\t"

    // move general purpose SIMD registers into context
    "movups	%xmm6,		  128(%rcx)\n\t"
    "movups	%xmm7,		  144(%rcx)\n\t"
    "movups	%xmm8,		  160(%rcx)\n\t"
    "movups	%xmm9,		  176(%rcx)\n\t"
    "movups	%xmm10,		  192(%rcx)\n\t"
    "movups	%xmm11,		  208(%rcx)\n\t"
    "movups	%xmm12,		  224(%rcx)\n\t"
    "movups	%xmm13,		  240(%rcx)\n\t"
    "movups	%xmm14,		  256(%rcx)\n\t"
    "movups	%xmm15,		  272(%rcx)\n\t"

    // move context into general purpose register
    "movq	  %rdx,		    %rax		 \n\t"
    "movq	  8(%rax),	  %rsp		 \n\t"
    "movq	  16(%rax),	  %rbp		 \n\t"

    // move input arguments into current context
    "movq	  24(%rax),	  %rcx		 \n\t"
    "movq	  32(%rax),	  %rdx		 \n\t"
    "movq	  40(%rax),	  %r8			 \n\t"
    "movq	  48(%rax),	  %r9			 \n\t"

    // move general purpose registers into current context
    "movq	  56(%rax),	  %r12		 \n\t"
    "movq	  64(%rax),	  %r13		 \n\t"
    "movq	  72(%rax),	  %r14		 \n\t"
    "movq	  80(%rax),	  %r15		 \n\t"
    "movq	  88(%rax),	  %rdi		 \n\t"
    "movq	  96(%rax),	  %rsi		 \n\t"
    "movq	  104(%rax),	%rbx		 \n\t"

    // move general purpose SIMD registers into current context
    "movups	128(%rax),	%xmm6		 \n\t"
    "movups	144(%rax),	%xmm7		 \n\t"
    "movups	160(%rax),	%xmm8		 \n\t"
    "movups	176(%rax),	%xmm9		 \n\t"
    "movups	192(%rax),	%xmm10	 \n\t"
    "movups	208(%rax),	%xmm11	 \n\t"
    "movups	224(%rax),	%xmm12	 \n\t"
    "movups	240(%rax),	%xmm13	 \n\t"
    "movups	256(%rax),	%xmm14	 \n\t"
    "movups	272(%rax),	%xmm15	 \n\t"

    "movq	  0(%rax),	  %rax		 \n\t"
    "jmp    *%rax                \n\t"

    "ret");

#elif defined(__unix__) || defined(__unix) || defined(__linux__)

__asm(
    ".text								        \n"
    ".align 4							        \n"
    "lbr_update_context:				  \n\t"
    "movq   (%rsp),     %rax		  \n\t"
    "movq	  %rax,		    0(%rdi)		\n\t"  // move RIP into context
    "leaq	  8(%rsp),	  %rax		  \n\t"
    "movq	  %rax,		    8(%rdi)		\n\t"  // move RSP into context
    "movq 	%rbp,		    16(%rdi)	\n\t"  // move RBP into context

    // move general purpose x64 registers into context
    "movq	  %r12,		    56(%rdi)	\n\t"
    "movq	  %r13,		    64(%rdi)	\n\t"
    "movq	  %r14,		    72(%rdi)	\n\t"
    "movq	  %r15,		    80(%rdi)	\n\t"
    "movq	  %rbx,		    104(%rdi)	\n\t"

    "ret");

__asm(
    ".text								       \n"
    ".align 4							       \n"
    "lbr_swap_context:					 \n\t"
    "movq	  (%rsp),		  %rcx		 \n\t"
    "movq	  %rcx,		    0(%rdi)	 \n\t"  // move RIP into context

    "leaq	  8(%rsp),	  %rcx		 \n\t"
    "movq	  %rcx,		    8(%rdi)	 \n\t"  // move RSP into context
    "movq 	%rbp,		    16(%rdi) \n\t"  // move RBP into context

    // move general purpose x64 registers into context
    "movq	  %r12,		    56(%rdi) \n\t"
    "movq	  %r13,		    64(%rdi) \n\t"
    "movq	  %r14,		    72(%rdi) \n\t"
    "movq	  %r15,		    80(%rdi) \n\t"
    "movq	  %rbx,		    88(%rdi) \n\t"

    // move context into general purpose register
    "movq	  %rsi,		    %r8		   \n\t"
    "movq	  16(%r8),	  %rbp		 \n\t"

    // move input arguments into current context
    "movq	  24(%r8),	  %rdi		 \n\t"
    "movq	  32(%r8),	  %rsi		 \n\t"

    // move general purpose registers into current context
    "movq	  40(%r8),	  %r12		 \n\t"
    "movq	  48(%r8),	  %r13		 \n\t"
    "movq	  56(%r8),	  %r14		 \n\t"
    "movq	  64(%r8),	  %r15		 \n\t"
    "movq	  72(%r8),	  %rbx		 \n\t"

    "movq	  8(%r8),	    %rsp		 \n\t"

    "movq	  0(%r8),	    %rcx		 \n\t"
    "jmp    *%rcx                \n\t"
    );

#endif

extern __attribute__((noinline)) void lbr_update_context(LbrFiberContext* p_context);
extern __attribute__((noinline)) void lbr_swap_context(LbrFiberContext* p_context_from, LbrFiberContext* p_context_to);

void lbrCreateFiber(LbrFiberCreateInfo* p_info, LbrFiber* p_fiber) {
  // https://gist.github.com/JiayinCao/07475d3423952b702d1efc5268b0df4e
  // https://stackoverflow.com/questions/71259613/c-fibers-crashing-on-printf
  usize stack_size = p_info->stack_size;

  p_fiber->alloc_callbacks = p_info->alloc_callbacks;
  p_fiber->fiber_id        = p_info->fiber_id;
  p_fiber->stack           = lbrAllocCallbacksAllocate(&p_info->alloc_callbacks, stack_size + STACK_ALIGNMENT - 1);

  uptr aligned_ptr = (uptr)p_fiber->stack;
  LBR_ALIGN(aligned_ptr, STACK_ALIGNMENT);

  LBR_ASSERT(((aligned_ptr & (STACK_ALIGNMENT - 1)) == 0));

  uptr* stack_top = (uptr*)(aligned_ptr + stack_size);

  p_fiber->stack_size      = stack_size;
  p_fiber->context.rip     = (void*)p_info->pfn_fiber_entry;
  p_fiber->context.rsp     = &stack_top[-3];
  stack_top[-2] = 0;
}

void lbrFiberConvertThread(LbrFiber* p_fiber) { lbr_update_context(&p_fiber->context); }

void lbrDestroyFiber(LbrFiber* p_fiber) { lbrAllocCallbacksFree(&p_fiber->alloc_callbacks, p_fiber->stack); }

void lbrFiberUpdate(LbrFiber* p_fiber) { lbr_update_context(&p_fiber->context); }

void lbrFiberSwap(LbrFiber* p_fiber_from, LbrFiber* p_fiber_to) {
  lbr_swap_context(&p_fiber_from->context, &p_fiber_to->context);
}

void lbrFiberSetToTask(LbrFiber* p_fiber, LbrTask* p_task) {
  p_fiber->context.rip = (uptr*)p_task->p_entry_point;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  p_fiber->context.rcx = p_task->p_first_arg;
  p_fiber->context.rdx = p_task->p_second_arg;

#elif defined(__unix__) || defined(__unix) || defined(__linux__)
  p_fiber->context.rdi = p_task->p_first_arg;
  p_fiber->context.rsi = p_task->p_second_arg;
#endif
}
