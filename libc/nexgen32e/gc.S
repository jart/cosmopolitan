/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/notice.inc"

	nop

//	Invokes deferred function calls.
//
//	This offers behavior similar to std::unique_ptr. Functions
//	overwrite their return addresses jumping here, and pushing
//	exactly one entry on the shadow stack below. Functions may
//	repeat that process multiple times, in which case the body
//	of this gadget loops and unwinds as a natural consequence.
//
//	@param	rax,rdx,xmm0,xmm1,st0,st1 is return value
//	@see	test/libc/runtime/gc_test.c
	.ftrace1
__gc:	.ftrace2

#ifdef __x86_64__

	mov	%fs:0,%rcx			// __get_tls()
	mov	0x18(%rcx),%rcx			// tls::garbages
	decl	(%rcx)				// --g->i
	mov	(%rcx),%r8d			// r8 = g->i
	mov	8(%rcx),%r9			// r9 = g->p
	js	9f
	shl	$5,%r8
	lea	(%r9,%r8),%r8
	mov	8(%r8),%r9
	mov	16(%r8),%rdi
	push	24(%r8)
	push	%rbp
	mov	%rsp,%rbp
	sub	$32,%rsp
	mov	%rax,-8(%rbp)
	mov	%rdx,-16(%rbp)
	movdqa	%xmm0,-32(%rbp)
	call	*%r9
	movdqa	-32(%rbp),%xmm0
	mov	-16(%rbp),%rdx
	mov	-8(%rbp),%rax
	leave
	ret
9:	ud2
	nop

#elif defined(__aarch64__)

//	if this code fails
//	check if CosmoTib's size changed
	sub	x8,x28,#192			// __get_tls()
	ldr	x9,[x8,0x18]			// tib::garbages
	ldr	x10,[x9]			// g->i
	ldr	x8,[x9,8]			// g->p
	sub	x10,x10,#1			//
	str	x10,[x9]			//
	sbfiz	x10,x10,5,32			//
	add	x8,x8,x10			// g->p+i
	ldr	x9,[x8,#8]			// g->p[i].fn
	ldr	x10,[x8,#16]			// g->p[i].arg
	ldr	x11,[x8,#24]			// g->p[i].ret
	stp	x29,x11,[sp,-208]!
	mov	x29,sp
	stp	x0,x1,[sp,16]
	stp	x2,x3,[sp,32]
	stp	x4,x5,[sp,48]
	stp	x6,x7,[sp,64]
	stp	q0,q1,[sp,80]
	stp	q2,q3,[sp,112]
	stp	q4,q5,[sp,144]
	stp	q6,q7,[sp,176]
	mov	x0,x10
	blr	x9
	ldp	q6,q7,[sp,176]
	ldp	q4,q5,[sp,144]
	ldp	q2,q3,[sp,112]
	ldp	q0,q1,[sp,80]
	ldp	x6,x7,[sp,64]
	ldp	x4,x5,[sp,48]
	ldp	x2,x3,[sp,32]
	ldp	x0,x1,[sp,16]
	ldp	x29,x30,[sp],208
	ret

#endif /* __x86_64__ */

	.endfn	__gc,globl,hidden
