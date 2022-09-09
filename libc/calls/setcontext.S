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

//	Sets machine state.
//
//	@see	getcontext()
setcontext:
	mov	224(%rdi),%rax
	test	%rax,%rax
	je	1f
	movaps	160(%rax),%xmm0
	movaps	176(%rax),%xmm1
	movaps	192(%rax),%xmm2
	movaps	208(%rax),%xmm3
	movaps	224(%rax),%xmm4
	movaps	240(%rax),%xmm5
	movaps	256(%rax),%xmm6
	movaps	272(%rax),%xmm7
	movaps	288(%rax),%xmm8
	movaps	304(%rax),%xmm9
	movaps	320(%rax),%xmm10
	movaps	336(%rax),%xmm11
	movaps	352(%rax),%xmm12
	movaps	368(%rax),%xmm13
	movaps	384(%rax),%xmm14
	movaps	400(%rax),%xmm15
1:	push	176(%rdi)
	popf
	mov	40(%rdi),%r8
	mov	48(%rdi),%r9
	mov	56(%rdi),%r10
	mov	64(%rdi),%r11
	mov	72(%rdi),%r12
	mov	80(%rdi),%r13
	mov	88(%rdi),%r14
	mov	96(%rdi),%r15
	mov	112(%rdi),%rsi
	mov	120(%rdi),%rbp
	mov	128(%rdi),%rbx
	mov	136(%rdi),%rdx
	mov	144(%rdi),%rax
	mov	152(%rdi),%rcx
	mov	160(%rdi),%rsp
	xor	%rax,%rax
	push	168(%rdi)
	mov	104(%rdi),%rdi
	ret
	.endfn	setcontext,globl

	.end
////////////////////////////////////////////////////////////////////////////////
noasan noubsan int setcontext(const ucontext_t *uc) {
  if (uc->uc_mcontext.fpregs) {
    asm volatile("movaps\t%0,%%xmm0" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[0]));
    asm volatile("movaps\t%0,%%xmm1" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[1]));
    asm volatile("movaps\t%0,%%xmm2" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[2]));
    asm volatile("movaps\t%0,%%xmm3" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[3]));
    asm volatile("movaps\t%0,%%xmm4" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[4]));
    asm volatile("movaps\t%0,%%xmm5" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[5]));
    asm volatile("movaps\t%0,%%xmm6" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[6]));
    asm volatile("movaps\t%0,%%xmm7" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[7]));
    asm volatile("movaps\t%0,%%xmm8" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[8]));
    asm volatile("movaps\t%0,%%xmm9" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[9]));
    asm volatile("movaps\t%0,%%xmm10" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[10]));
    asm volatile("movaps\t%0,%%xmm11" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[11]));
    asm volatile("movaps\t%0,%%xmm12" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[12]));
    asm volatile("movaps\t%0,%%xmm13" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[13]));
    asm volatile("movaps\t%0,%%xmm14" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[14]));
    asm volatile("movaps\t%0,%%xmm15" : /* no outputs */ : "m"(uc->uc_mcontext.fpregs->xmm[15]));
  }
  asm volatile("mov\t%0,%%r8" : /* no outputs */ : "m"(uc->uc_mcontext.r8));
  asm volatile("mov\t%0,%%r9" : /* no outputs */ : "m"(uc->uc_mcontext.r9));
  asm volatile("mov\t%0,%%r10" : /* no outputs */ : "m"(uc->uc_mcontext.r10));
  asm volatile("mov\t%0,%%r11" : /* no outputs */ : "m"(uc->uc_mcontext.r11));
  asm volatile("mov\t%0,%%r12" : /* no outputs */ : "m"(uc->uc_mcontext.r12));
  asm volatile("mov\t%0,%%r13" : /* no outputs */ : "m"(uc->uc_mcontext.r13));
  asm volatile("mov\t%0,%%r14" : /* no outputs */ : "m"(uc->uc_mcontext.r14));
  asm volatile("mov\t%0,%%r15" : /* no outputs */ : "m"(uc->uc_mcontext.r15));
  asm volatile("mov\t%0,%%rsi" : /* no outputs */ : "m"(uc->uc_mcontext.rsi));
  asm volatile("mov\t%0,%%rbp" : /* no outputs */ : "m"(uc->uc_mcontext.rbp));
  asm volatile("mov\t%0,%%rbx" : /* no outputs */ : "m"(uc->uc_mcontext.rbx));
  asm volatile("mov\t%0,%%rdx" : /* no outputs */ : "m"(uc->uc_mcontext.rdx));
  asm volatile("mov\t%0,%%rax" : /* no outputs */ : "m"(uc->uc_mcontext.rax));
  asm volatile("mov\t%0,%%rcx" : /* no outputs */ : "m"(uc->uc_mcontext.rcx));
  asm volatile("mov\t%0,%%rsp" : /* no outputs */ : "m"(uc->uc_mcontext.rsp));
  asm volatile("xor\t%%rax,%%rax\n\t"
               "push\t%0\n\t"
               "mov\t%1,%%rdi\n\t"
               "ret"
               : /* no outputs */
               : "m"(uc->uc_mcontext.rip), "m"(uc->uc_mcontext.rdi));
  unreachable;
}
