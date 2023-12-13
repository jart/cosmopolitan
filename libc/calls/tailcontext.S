/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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

//	tailed called by setcontext() implementation
__tailcontext:
#ifdef __x86_64__

	lea	608(%rdi),%rax
	movaps	-0x80(%rax),%xmm0
	movaps	-0x70(%rax),%xmm1
	movaps	-0x60(%rax),%xmm2
	movaps	-0x50(%rax),%xmm3
	movaps	-0x40(%rax),%xmm4
	movaps	-0x30(%rax),%xmm5
	movaps	-0x20(%rax),%xmm6
	movaps	-0x10(%rax),%xmm7
	movaps	0x00(%rax),%xmm8
	movaps	0x10(%rax),%xmm9
	movaps	0x20(%rax),%xmm10
	movaps	0x30(%rax),%xmm11
	movaps	0x40(%rax),%xmm12
	movaps	0x50(%rax),%xmm13
	movaps	0x60(%rax),%xmm14
	movaps	0x70(%rax),%xmm15

	lea	80(%rdi),%rax
	mov	-40(%rax),%r8
	mov	-32(%rax),%r9
	mov	-24(%rax),%r10
	mov	-16(%rax),%r11
	mov	-8(%rax),%r12
	mov	0(%rax),%r13
	mov	8(%rax),%r14
	mov	16(%rax),%r15
	mov	32(%rax),%rsi
	mov	40(%rax),%rbp
	mov	48(%rax),%rbx
	mov	56(%rax),%rdx
	mov	72(%rax),%rcx
	mov	80(%rax),%rsp
	push	88(%rax)
	mov	24(%rax),%rdi

	xor	%eax,%eax
	ret

#elif defined(__aarch64__)
#define REGS(i) 184+i*8

/*void setfpsimd(const ucontext_t *uc) {
  struct fpsimd_context *fp;
  fp = (struct fpsimd_context *)uc->uc_mcontext.__reserved;
  if (fp[0].head.magic == FPSIMD_MAGIC) {
    asm("msr\tfpsr,%0" ::"r"(fp->fpsr));
    asm("msr\tfpcr,%0" ::"r"(fp->fpcr));
    asm("ldp\tq0,q1,%0" ::"m"(fp->vregs[0]));
    asm("ldp\tq2,q3,%0" ::"m"(fp->vregs[2]));
    asm("ldp\tq4,q5,%0" ::"m"(fp->vregs[4]));
    asm("ldp\tq6,q7,%0" ::"m"(fp->vregs[6]));
    asm("ldp\tq8,q9,%0" ::"m"(fp->vregs[8]));
    asm("ldp\tq10,q11,%0" ::"m"(fp->vregs[10]));
    asm("ldp\tq12,q13,%0" ::"m"(fp->vregs[12]));
    asm("ldp\tq14,q15,%0" ::"m"(fp->vregs[14]));
  }
}*/

	ldr	w14,[x0,464]
	add	x15,x0,464
	mov	w13,0x8001
	movk	w13,0x4650,lsl 16
	cmp	w14,w13
	bne	1f
	ldr	w13,[x15,8]
	msr	fpsr,x13
	ldr	w13,[x15,12]
	msr	fpcr,x13
	ldp	q0,q1,[x15,16]
	ldp	q2,q3,[x15,48]
	ldp	q4,q5,[x15,80]
	ldp	q6,q7,[x15,112]
	ldp	q8,q9,[x15,144]
	ldp	q10,q11,[x15,176]
	ldp	q12,q13,[x15,208]
	ldp	q14,q15,[x15,240]
1:

//	x16 is clobbered
//	x18 belongs to apple
//	all other registers are restored

	ldp	x1,x16,[x0,REGS(31)]		// sp, pc
	mov	sp,x1
	ldr	x30,[x0,REGS(30)]
	ldp	x28,x29,[x0,REGS(28)]
	ldp	x26,x27,[x0,REGS(26)]
	ldp	x24,x25,[x0,REGS(24)]
	ldp	x22,x23,[x0,REGS(22)]
	ldp	x20,x21,[x0,REGS(20)]
	ldr	x19,[x0,REGS(19)]
	ldr	x17,[x0,REGS(17)]
	ldp	x14,x15,[x0,REGS(14)]
	ldp	x12,x13,[x0,REGS(12)]
	ldp	x10,x11,[x0,REGS(10)]
	ldp	x8,x9,[x0,REGS(8)]
	ldp	x6,x7,[x0,REGS(6)]
	ldp	x4,x5,[x0,REGS(4)]
	ldp	x2,x3,[x0,REGS(2)]
	ldp	x0,x1,[x0,REGS(0)]
	br	x16

#else
#error "unsupported architecture"
#endif
	.endfn	__tailcontext,globl
