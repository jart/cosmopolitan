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
#include "libc/macros.h"
#include "libc/dce.h"
#include "libc/notice.inc"
#define INITIAL_CAPACITY 4

/	Invokes deferred function calls.
/
/	This offers behavior similar to std::unique_ptr. Functions
/	overwrite their return addresses jumping here, and pushing
/	exactly one entry on the shadow stack below. Functions may
/	repeat that process multiple times, in which case the body
/	of this gadget loops and unwinds as a natural consequence.
/
/	@param	rax,rdx,xmm0,xmm1,st0,st1 is return value
/	@see	test/libc/runtime/gc_test.c
/	<LIMBO>
__gc:	decq	__garbage(%rip)
	mov	__garbage(%rip),%r8
	mov	__garbage+16(%rip),%r9
	js	9f
	shl	$5,%r8
	lea	(%r9,%r8),%r8
	mov	8(%r8),%r9
	mov	16(%r8),%rdi
	push	24(%r8)
/	</LIMBO>
	push	%rbp
	mov	%rsp,%rbp
	sub	$0x20,%rsp
	push	%rax
	push	%rdx
	movdqa	%xmm0,-0x20(%rbp)
	movdqa	%xmm1,-0x10(%rbp)
	call	*%r9
	movdqa	-0x10(%rbp),%xmm1
	movdqa	-0x20(%rbp),%xmm0
	pop	%rdx
	pop	%rax
	leave
	ret
9:	call	abort
	.endfn	__gc,globl,hidden
	.source	__FILE__

	.bss
	.align	8
__garbage:
	.quad	0				# garbage.i
	.quad	0				# garbage.n
	.quad	0				# garbage.p
	.rept	INITIAL_CAPACITY
	.quad	0				# garbage.p[𝑖].frame
	.quad	0				# garbage.p[𝑖].fn
	.quad	0				# garbage.p[𝑖].arg
	.quad	0				# garbage.p[𝑖].ret
	.endr
	.endobj	__garbage,globl,hidden
	.previous

	.init.start 100,_init_garbage
	movb	$INITIAL_CAPACITY,__garbage+8(%rip)
	movl	$__garbage+24,__garbage+16(%rip)
	.init.end 100,_init_garbage
