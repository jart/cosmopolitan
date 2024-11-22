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

//	@fileoverview textual include for loading processor state

#ifdef __x86_64__

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
	mov	%rcx,152(%rdi)
	lea	8(%rsp),%rax
	mov	%rax,160(%rdi)		// rsp = caller's rsp
	mov	(%rsp),%rax
	mov	%rax,168(%rdi)		// rip = return address

	lea	608(%rdi),%rax
	movaps	%xmm0,-0x80(%rax)
	movaps	%xmm1,-0x70(%rax)
	movaps	%xmm2,-0x60(%rax)
	movaps	%xmm3,-0x50(%rax)
	movaps	%xmm4,-0x40(%rax)
	movaps	%xmm5,-0x30(%rax)
	movaps	%xmm6,-0x20(%rax)
	movaps	%xmm7,-0x10(%rax)
	movaps	%xmm8,0x00(%rax)
	movaps	%xmm9,0x10(%rax)
	movaps	%xmm10,0x20(%rax)
	movaps	%xmm11,0x30(%rax)
	movaps	%xmm12,0x40(%rax)
	movaps	%xmm13,0x50(%rax)
	movaps	%xmm14,0x60(%rax)
	movaps	%xmm15,0x70(%rax)
	lea	320(%rdi),%rax		// rax = &__fpustate
	mov	%rax,224(%rdi)		// fpregs = rax

#elif defined(__aarch64__)
#define REGS(i) 184+i*8

//	x14 and x15 are clobbered
//	all other registers are preserved

	stp	xzr,x1,[x0,REGS(0)]	// context.x0 = 0
	stp	x2,x3,[x0,REGS(2)]
	stp	x4,x5,[x0,REGS(4)]
	stp	x6,x7,[x0,REGS(6)]
	stp	x8,x9,[x0,REGS(8)]
	stp	x10,x11,[x0,REGS(10)]
	stp	x12,x13,[x0,REGS(12)]
	stp	x14,x15,[x0,REGS(14)]
	stp	x16,x17,[x0,REGS(16)]
	stp	x18,x19,[x0,REGS(18)]
	stp	x20,x21,[x0,REGS(20)]
	stp	x22,x23,[x0,REGS(22)]
	stp	x24,x25,[x0,REGS(24)]
	stp	x26,x27,[x0,REGS(26)]
	stp	x28,x29,[x0,REGS(28)]
	str	x30,[x0,REGS(30)]
	mov	x15,sp
	stp	x15,x30,[x0,REGS(31)]	// sp, pc = caller sp, ret address
	str	xzr,[x0,448]		// pstate = 0
	str	xzr,[x0,456]		// no vectors yet

/*void getfpsimd(ucontext_t *uc) {
  struct fpsimd_context *fp;
  fp = (struct fpsimd_context *)uc->uc_mcontext.__reserved;
  fp[0].head.magic = FPSIMD_MAGIC;
  fp[0].head.size = sizeof(*fp);
  fp[1].head.magic = 0;
  fp[1].head.size = 0;
  asm("mrs\t%0,fpsr" : "=r"(fp->fpsr));
  asm("mrs\t%0,fpcr" : "=r"(fp->fpcr));
  asm("stp\tq0,q1,%0" : "=m"(fp->vregs[0]));
  asm("stp\tq2,q3,%0" : "=m"(fp->vregs[2]));
  asm("stp\tq4,q5,%0" : "=m"(fp->vregs[4]));
  asm("stp\tq6,q7,%0" : "=m"(fp->vregs[6]));
  asm("stp\tq8,q9,%0" : "=m"(fp->vregs[8]));
  asm("stp\tq10,q11,%0" : "=m"(fp->vregs[10]));
  asm("stp\tq12,q13,%0" : "=m"(fp->vregs[12]));
  asm("stp\tq14,q15,%0" : "=m"(fp->vregs[14]));
}*/

	add	x15,x0,464
	mov	x14,0x8001
	movk	x14,0x4650,lsl 16
	str	xzr,[x0,992]
	movk	x14,0x210,lsl 32
	str	x14,[x0,464]
	mrs	x14,fpsr
	str	w14,[x15,8]
	mrs	x14,fpcr
	str	w14,[x15,12]
	stp	q0,q1,[x15,16]
	stp	q2,q3,[x15,48]
	stp	q4,q5,[x15,80]
	stp	q6,q7,[x15,112]
	stp	q8,q9,[x15,144]
	stp	q10,q11,[x15,176]
	stp	q12,q13,[x15,208]
	stp	q14,q15,[x15,240]

#else
#error "unsupported architecture"
#endif
