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

//	Jumps up stack to previous setjmp() invocation.
//
//	This is the same as longjmp() but also unwinds the stack to free
//	memory, etc. that was registered using gc() or defer(). If GC
//	isn't linked, this behaves the same as longjmp().
//
//	@param	rdi points to the jmp_buf which must be the same stack
//	@param	esi is returned by setjmp() invocation (coerced nonzero)
//	@assume	system five nexgen32e abi conformant
//	@see	examples/ctrlc.c
//	@noreturn
	.ftrace1
gclongjmp:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%fs:0,%r12			// __get_tls()
	mov	0x18(%r12),%r12			// Tls::garbages
	test	%r12,%r12
	jz	0f
	movl	(%r12),%r13d			// garbages.i
	test	%r13d,%r13d
	jnz	.L.unwind.destructors
0:	jmp	longjmp
.L.unwind.destructors:
	push	%rdi
	push	%rsi
	mov	8(%r12),%r14			// garbages.p
	mov	(%rdi),%r15			// jmp_buf[0] is new %rsp
	shl	$5,%r13				// log2(sizeof(struct Garbage))
1:	sub	$32,%r13			// 𝑖--
	js	2f
	cmp	(%r14,%r13),%r15		// new %rsp > garbages.p[𝑖].frame
	jbe	2f
	mov	16(%r14,%r13),%rdi		// garbages.p[𝑖].arg
	callq	*8(%r14,%r13)			// garbages.p[𝑖].fn
	decl	(%r12)				// garbages.i--
	jmp	1b
2:	pop	%rsi
	pop	%rdi
	jmp	0b
#elif defined(__aarch64__)
	b	longjmp
#else
#error "unsupported architecture"
#endif /* __x86_64__ */
	.endfn	gclongjmp,globl
