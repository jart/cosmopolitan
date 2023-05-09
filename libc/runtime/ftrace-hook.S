/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
.privileged

ftrace_hook:
#ifdef __x86_64__

	cmp	$0,__ftrace(%rip)
	jg	1f
	ret
1:	push	%rbp
	mov	%rsp,%rbp
	and	$-16,%rsp
	sub	$0x80,%rsp
	movaps	%xmm0,0x00(%rsp)
	movaps	%xmm1,0x10(%rsp)
	movaps	%xmm2,0x20(%rsp)
	movaps	%xmm3,0x30(%rsp)
	movaps	%xmm4,0x40(%rsp)
	movaps	%xmm5,0x50(%rsp)
	movaps	%xmm6,0x60(%rsp)
	movaps	%xmm7,0x70(%rsp)
	push	%rax
	push	%rax
	push	%rdi
	push	%rsi
	push	%rdx
	push	%rcx
	push	%r8
	push	%r9
	push	%r10
	push	%r11
	call	ftracer
	pop	%r11
	pop	%r10
	pop	%r9
	pop	%r8
	pop	%rcx
	pop	%rdx
	pop	%rsi
	pop	%rdi
	pop	%rax
	pop	%rax
	movaps	0x00(%rsp),%xmm0
	movaps	0x10(%rsp),%xmm1
	movaps	0x20(%rsp),%xmm2
	movaps	0x30(%rsp),%xmm3
	movaps	0x40(%rsp),%xmm4
	movaps	0x50(%rsp),%xmm5
	movaps	0x60(%rsp),%xmm6
	movaps	0x70(%rsp),%xmm7
	leave

#elif defined(__aarch64__)

	stp	x0,x1,[sp,#-16]!
	stp	x2,x3,[sp,#-16]!
	stp	x4,x5,[sp,#-16]!
	stp	x6,x7,[sp,#-16]!
	stp	x8,x9,[sp,#-16]!
	stp	x10,x11,[sp,#-16]!
	stp	x12,x13,[sp,#-16]!
	stp	x14,x15,[sp,#-16]!
	stp	x16,x17,[sp,#-16]!
	stp	x18,x19,[sp,#-16]!
	stp	x20,x21,[sp,#-16]!
	stp	x22,x23,[sp,#-16]!
	stp	x24,x25,[sp,#-16]!
	stp	x26,x27,[sp,#-16]!
	stp	x28,x29,[sp,#-16]!
	str	x30,[sp,#-16]!
	bl	ftracer
	ldr	x30,[sp,#16]!
	ldp	x28,x29,[sp,#16]!
	ldp	x26,x27,[sp,#16]!
	ldp	x24,x25,[sp,#16]!
	ldp	x22,x23,[sp,#16]!
	ldp	x20,x21,[sp,#16]!
	ldp	x18,x19,[sp,#16]!
	ldp	x16,x17,[sp,#16]!
	ldp	x14,x15,[sp,#16]!
	ldp	x12,x13,[sp,#16]!
	ldp	x10,x11,[sp,#16]!
	ldp	x8,x9,[sp,#16]!
	ldp	x6,x7,[sp,#16]!
	ldp	x4,x5,[sp,#16]!
	ldp	x2,x3,[sp,#16]!
	ldp	x0,x1,[sp,#16]!

#endif /* __x86_64__ */
	ret
	.endfn	ftrace_hook,globl
