/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                   :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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

#define SIZE 0x0200
#define SKEW 0x10

foreign_tramp:

#ifdef __x86_64__

	push	%rbp
	mov	%rsp,%rbp
	sub	$SIZE,%rsp

//	save vector arguments
	movdqu	%xmm0,-0x10(%rbp)
	movdqu	%xmm1,-0x20(%rbp)
	movdqu	%xmm2,-0x30(%rbp)
	movdqu	%xmm3,-0x40(%rbp)
	movdqu	%xmm4,-0x50(%rbp)
	movdqu	%xmm5,-0x60(%rbp)
	movdqu	%xmm6,-0x70(%rbp)
	movdqu	%xmm7,-0x80(%rbp)

//	save register arguments
	mov	%rdi,-0x88(%rbp)
	mov	%rsi,-0x90(%rbp)
	mov	%rdx,-0x98(%rbp)
	mov	%rcx,-0xa0(%rbp)
	mov	%r8,-0xa8(%rbp)
	mov	%r9,-0xb0(%rbp)

//	save function pointer
	mov	%rax,-0xb8(%rbp)

//	block signals
	call	__sig_block
	mov	%rax,-0xc0(%rbp)

//	switch to foreign tls
	mov	%fs:0,%rax
	mov	%rax,-0xc8(%rbp)
	mov	__foreign+8(%rip),%rdi
	call	__set_tls

//	move stack arguments
	movdqu	SKEW+0x00(%rbp),%xmm0
	movdqu	SKEW+0x10(%rbp),%xmm1
	movdqu	SKEW+0x20(%rbp),%xmm2
	movdqu	SKEW+0x30(%rbp),%xmm3
	movdqu	SKEW+0x40(%rbp),%xmm4
	movdqu	SKEW+0x50(%rbp),%xmm5
	movdqu	SKEW+0x60(%rbp),%xmm6
	movdqu	SKEW+0x70(%rbp),%xmm7
	movdqu	%xmm0,-SIZE+0x00(%rbp)
	movdqu	%xmm1,-SIZE+0x10(%rbp)
	movdqu	%xmm2,-SIZE+0x20(%rbp)
	movdqu	%xmm3,-SIZE+0x30(%rbp)
	movdqu	%xmm4,-SIZE+0x40(%rbp)
	movdqu	%xmm5,-SIZE+0x50(%rbp)
	movdqu	%xmm6,-SIZE+0x60(%rbp)
	movdqu	%xmm7,-SIZE+0x70(%rbp)
	movdqu	SKEW+0x80(%rbp),%xmm0
	movdqu	SKEW+0x90(%rbp),%xmm1
	movdqu	SKEW+0xa0(%rbp),%xmm2
	movdqu	SKEW+0xb0(%rbp),%xmm3
	movdqu	SKEW+0xc0(%rbp),%xmm4
	movdqu	SKEW+0xd0(%rbp),%xmm5
	movdqu	SKEW+0xe0(%rbp),%xmm6
	movdqu	SKEW+0xf0(%rbp),%xmm7
	movdqu	%xmm0,-SIZE+0x80(%rbp)
	movdqu	%xmm1,-SIZE+0x90(%rbp)
	movdqu	%xmm2,-SIZE+0xa0(%rbp)
	movdqu	%xmm3,-SIZE+0xb0(%rbp)
	movdqu	%xmm4,-SIZE+0xc0(%rbp)
	movdqu	%xmm5,-SIZE+0xd0(%rbp)
	movdqu	%xmm6,-SIZE+0xe0(%rbp)
	movdqu	%xmm7,-SIZE+0xf0(%rbp)

//	restore vector arguments
	movdqu	-0x10(%rbp),%xmm0
	movdqu	-0x20(%rbp),%xmm1
	movdqu	-0x30(%rbp),%xmm2
	movdqu	-0x40(%rbp),%xmm3
	movdqu	-0x50(%rbp),%xmm4
	movdqu	-0x60(%rbp),%xmm5
	movdqu	-0x70(%rbp),%xmm6
	movdqu	-0x80(%rbp),%xmm7

//	restore register arguments
	mov	-0x88(%rbp),%rdi
	mov	-0x90(%rbp),%rsi
	mov	-0x98(%rbp),%rdx
	mov	-0xa0(%rbp),%rcx
	mov	-0xa8(%rbp),%r8
	mov	-0xb0(%rbp),%r9

//	call function
	mov	-0xb8(%rbp),%rax
	call	*%rax

//	save function result
	movdqu	%xmm0,-0x10(%rbp)
	movdqu	%xmm1,-0x20(%rbp)
	mov	%rax,-0x28(%rbp)
	mov	%rdx,-0x30(%rbp)

//	restore tls
	mov	-0xc8(%rbp),%rdi
	call	__set_tls

//	unblock signals
	mov	-0xc0(%rbp),%rdi
	call	__sig_unblock

//	restore function result
	mov	-0x28(%rbp),%rax
	mov	-0x30(%rbp),%rdx
	movdqu	-0x10(%rbp),%xmm0
	movdqu	-0x20(%rbp),%xmm1

	add	$SIZE,%rsp
	pop	%rbp
	ret

#elif defined(__aarch64__)

	stp	x29,x30,[sp,-0x100]!
	mov	x29,sp

//	save vector arguments
	stp	q0,q1,[sp,0x10]
	stp	q2,q3,[sp,0x30]
	stp	q4,q5,[sp,0x50]
	stp	q6,q7,[sp,0x70]

//	save register arguments
	stp	x0,x1,[sp,0x90]
	stp	x2,x3,[sp,0xa0]
	stp	x4,x5,[sp,0xb0]
	stp	x6,x7,[sp,0xc0]

//	save function pointer
	str	x8,[sp,0xd0]

//	block signals
	bl	__sig_block
	str	x0,[sp,0xd8]

//	move stack arguments
	sub	sp,sp,#0x100
	ldp	q0,q1,[sp,SIZE+0x00]
	ldp	q2,q3,[sp,SIZE+0x20]
	ldp	q4,q5,[sp,SIZE+0x40]
	ldp	q6,q7,[sp,SIZE+0x60]
	stp	q0,q1,[sp,0x00]
	stp	q2,q3,[sp,0x20]
	stp	q4,q5,[sp,0x40]
	stp	q6,q7,[sp,0x60]
	ldp	q0,q1,[sp,SIZE+0x80]
	ldp	q2,q3,[sp,SIZE+0xa0]
	ldp	q4,q5,[sp,SIZE+0xc0]
	ldp	q6,q7,[sp,SIZE+0xe0]
	stp	q0,q1,[sp,0x80]
	stp	q2,q3,[sp,0xa0]
	stp	q4,q5,[sp,0xc0]
	stp	q6,q7,[sp,0xe0]

//	restore vector arguments
	ldp	q0,q1,[sp,0x100+0x10]
	ldp	q2,q3,[sp,0x100+0x30]
	ldp	q4,q5,[sp,0x100+0x50]
	ldp	q6,q7,[sp,0x100+0x70]

//	restore register arguments
	ldp	x0,x1,[sp,0x100+0x90]
	ldp	x2,x3,[sp,0x100+0xa0]
	ldp	x4,x5,[sp,0x100+0xb0]
	ldp	x6,x7,[sp,0x100+0xc0]

//	call function
	ldr	x8,[sp,0x100+0xd0]
	blr	x8
	add	sp,sp,#0x100

//	save vector results
	stp	q0,q1,[sp,0x10]
	stp	q2,q3,[sp,0x30]
	stp	q4,q5,[sp,0x50]
	stp	q6,q7,[sp,0x70]

//	save register results
	stp	x0,x1,[sp,0x90]
	stp	x2,x3,[sp,0xa0]
	stp	x4,x5,[sp,0xb0]
	stp	x6,x7,[sp,0xc0]

//	unblock signals
	ldr	x0,[sp,0xd8]
	bl	__sig_unblock

//	restore vector results
	ldp	q0,q1,[sp,0x10]
	ldp	q2,q3,[sp,0x30]
	ldp	q4,q5,[sp,0x50]
	ldp	q6,q7,[sp,0x70]

//	restore register results
	ldp	x0,x1,[sp,0x90]
	ldp	x2,x3,[sp,0xa0]
	ldp	x4,x5,[sp,0xb0]
	ldp	x6,x7,[sp,0xc0]

	ldp	x29,x30,[sp],0x100
	ret

#endif // __x86_64__

	.endfn	foreign_tramp,globl
