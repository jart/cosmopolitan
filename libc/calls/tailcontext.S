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
	mov	152(%rdi),%rcx
	mov	160(%rdi),%rsp
	push	168(%rdi)
	mov	104(%rdi),%rdi

	xor	%eax,%eax
	ret

#elif defined(__aarch64__)
#define REGS(i) 184+i*8

	ldp	x1,x16,[x0,REGS(31)]		// sp, pc
	mov	sp,x1
	ldr	x30,[x0,REGS(30)]
	ldp	x28,x29,[x0,REGS(28)]
	ldp	x26,x27,[x0,REGS(26)]
	ldp	x24,x25,[x0,REGS(24)]
	ldp	x22,x23,[x0,REGS(22)]
	ldp	x20,x21,[x0,REGS(20)]
	ldp	x18,x19,[x0,REGS(18)]
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
