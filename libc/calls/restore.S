/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
.text.windows

//	Restores thread to state before signal.
//
//	@param	rdi	points to ucontext_t with machine state
//	@noreturn
__sig_restore:

//	restore vector registers
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

//	restore general registers
	lea	80(%rdi),%rax
	mov	-40(%rax),%r8
	mov	-32(%rax),%r9
	mov	-24(%rax),%r10
	mov	-16(%rax),%r11
	mov	-8(%rax),%r12
	mov	0(%rax),%r13
	mov	8(%rax),%r14
	mov	16(%rax),%r15
	mov	24(%rax),%rdi
	mov	32(%rax),%rsi
	mov	48(%rax),%rbx
	mov	56(%rax),%rdx
	mov	72(%rax),%rcx
	mov	40(%rax),%rbp
	mov	80(%rax),%rsp

//	this clobbers the red zone
	push	88(%rax)		// rip
	push	64(%rax)		// rax
	push	96(%rax)		// flags
	popf
	pop	%rax
	ret

	.endfn	__sig_restore,globl
