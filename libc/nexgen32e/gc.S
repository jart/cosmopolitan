/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
//	@threadsafe
__gc:	mov	%fs:0,%rcx			// __get_tls()
	mov	0x18(%rcx),%rcx			// tls::garbages
	decl	(%rcx)				// ++g->i
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
	.endfn	__gc,globl,hidden
