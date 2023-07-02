/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"

//	Gets machine state.
//
//	@see	setcontext()
getcontext:
#ifdef __x86_64__

	pushf
	pop	176(%rdi)
	movaps	%xmm0,480(%rdi)
	movaps	%xmm1,496(%rdi)
	movaps	%xmm2,512(%rdi)
	movaps	%xmm3,528(%rdi)
	movaps	%xmm4,544(%rdi)
	movaps	%xmm5,560(%rdi)
	movaps	%xmm6,576(%rdi)
	movaps	%xmm7,592(%rdi)
	movaps	%xmm8,608(%rdi)
	movaps	%xmm9,624(%rdi)
	movaps	%xmm10,640(%rdi)
	movaps	%xmm11,656(%rdi)
	movaps	%xmm12,672(%rdi)
	movaps	%xmm13,688(%rdi)
	movaps	%xmm14,704(%rdi)
	movaps	%xmm15,720(%rdi)
	mov	%r8,40(%rdi)
	mov	%r9,48(%rdi)
	mov	%r10,56(%rdi)
	mov	%r11,64(%rdi)
	mov	%r12,72(%rdi)
	mov	%r13,80(%rdi)
	mov	%r14,88(%rdi)
	mov	%r15,96(%rdi)
	mov	%rdi,104(%rdi)
	mov	%rsi,112(%rdi)
	mov	%rbp,120(%rdi)
	mov	%rbx,128(%rdi)
	mov	%rdx,136(%rdi)
	mov	%rax,144(%rdi)
	mov	%rcx,152(%rdi)
	lea	320(%rdi),%rax
	mov	%rax,224(%rdi)
	lea	8(%rsp),%rax
	mov	%rax,160(%rdi)
	mov	(%rsp),%rax
	mov	%rax,168(%rdi)
	jmp	__getcontext

#elif defined(__aarch64__)

	stp	x0,x1,[x0,184]
	stp	x2,x3,[x0,200]
	stp	x4,x5,[x0,216]
	stp	x6,x7,[x0,232]
	stp	x8,x9,[x0,248]
	stp	x10,x11,[x0,264]
	stp	x12,x13,[x0,280]
	stp	x14,x15,[x0,296]
	stp	x16,x17,[x0,312]
	stp	x18,x19,[x0,328]
	stp	x20,x21,[x0,344]
	stp	x22,x23,[x0,360]
	stp	x24,x25,[x0,376]
	stp	x26,x27,[x0,392]
	stp	x28,x29,[x0,408]
	str	x30,[x0,424]
	mov	x1,sp
	str	x1,[x0,432]		// sp = caller's sp
	str	x30,[x0,440]		// pc = caller's pc
	b	__getcontext

#else
#error "unsupported architecture"
#endif
	.endfn	getcontext,globl

	.end
////////////////////////////////////////////////////////////////////////////////

noasan noubsan dontinstrument int getcontext(ucontext_t *uc) {
  asm volatile("movaps\t%%xmm0,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[0]));
  asm volatile("movaps\t%%xmm1,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[1]));
  asm volatile("movaps\t%%xmm2,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[2]));
  asm volatile("movaps\t%%xmm3,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[3]));
  asm volatile("movaps\t%%xmm4,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[4]));
  asm volatile("movaps\t%%xmm5,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[5]));
  asm volatile("movaps\t%%xmm6,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[6]));
  asm volatile("movaps\t%%xmm7,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[7]));
  asm volatile("movaps\t%%xmm8,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[8]));
  asm volatile("movaps\t%%xmm9,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[9]));
  asm volatile("movaps\t%%xmm10,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[10]));
  asm volatile("movaps\t%%xmm11,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[11]));
  asm volatile("movaps\t%%xmm12,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[12]));
  asm volatile("movaps\t%%xmm13,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[13]));
  asm volatile("movaps\t%%xmm14,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[14]));
  asm volatile("movaps\t%%xmm15,%0" : /* no outputs */ : "m"(uc->__fpustate.xmm[15]));
  asm volatile("mov\t%%r8,%0" : "=m"(uc->uc_mcontext.r8));
  asm volatile("mov\t%%r9,%0" : "=m"(uc->uc_mcontext.r9));
  asm volatile("mov\t%%r10,%0" : "=m"(uc->uc_mcontext.r10));
  asm volatile("mov\t%%r11,%0" : "=m"(uc->uc_mcontext.r11));
  asm volatile("mov\t%%r12,%0" : "=m"(uc->uc_mcontext.r12));
  asm volatile("mov\t%%r13,%0" : "=m"(uc->uc_mcontext.r13));
  asm volatile("mov\t%%r14,%0" : "=m"(uc->uc_mcontext.r14));
  asm volatile("mov\t%%r15,%0" : "=m"(uc->uc_mcontext.r15));
  asm volatile("mov\t%%rdi,%0" : "=m"(uc->uc_mcontext.rdi));
  asm volatile("mov\t%%rsi,%0" : "=m"(uc->uc_mcontext.rsi));
  asm volatile("mov\t%%rbp,%0" : "=m"(uc->uc_mcontext.rbp));
  asm volatile("mov\t%%rbx,%0" : "=m"(uc->uc_mcontext.rbx));
  asm volatile("mov\t%%rdx,%0" : "=m"(uc->uc_mcontext.rdx));
  asm volatile("mov\t%%rax,%0" : "=m"(uc->uc_mcontext.rax));
  asm volatile("mov\t%%rcx,%0" : "=m"(uc->uc_mcontext.rcx));
  uc->uc_mcontext.fpregs = &uc->__fpustate;
  asm volatile("lea\t8(%%rsp),%%rax\n\t"
               "mov\t%%rax,%0"
               : "=m"(uc->uc_mcontext.rsp)
               : /* no inputs */
               : "rax");
  asm volatile("mov\t(%%rsp),%%rax\n\t"
               "mov\t%%rax,%0"
               : "=m"(uc->uc_mcontext.rip)
               : /* no inputs */
               : "rax");
  return 0;
}

noasan noubsan dontinstrument int getcontext(ucontext_t *uc) {
  asm volatile("stp\tx0,x1,%0" : "=m"(uc->uc_mcontext.regs[0]));
  asm volatile("stp\tx2,x3,%0" : "=m"(uc->uc_mcontext.regs[2]));
  asm volatile("mov\tx1,sp\n\tstr\tx1,%0" : "=m"(uc->uc_mcontext.sp));
  asm volatile("str\tx30,%0" : "=m"(uc->uc_mcontext.pc));
  return 0;
}
