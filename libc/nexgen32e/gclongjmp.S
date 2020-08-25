/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.h"

/	Jumps up stack to previous setjmp() invocation.
/
/	This is the same as longjmp() but also unwinds the stack to free
/	memory, etc. that was registered using gc() or defer(). If GC
/	isn't linked, this behaves the same as longjmp().
/
/	@param	rdi points to the jmp_buf which must be the same stack
/	@param	esi is returned by setjmp() invocation (coerced nonzero)
/	@assume	system five nexgen32e abi conformant
/	@see	examples/ctrlc.c
/	@noreturn
gclongjmp:
	.leafprologue
	.profilable
	.weak	g_garbage
	lea	g_garbage(%rip),%r12
	test	%r12,%r12
	jnz	.L.unwind.destructors
0:	jmp	longjmp
.L.unwind.destructors:
	push	%rdi
	push	%rsi
	mov	(%r12),%r13			# garbage.i
	mov	16(%r12),%r14			# garbage.p
	mov	(%rdi),%r15			# jmp_buf[0] is new %rsp
	shl	$5,%r13
1:	test	%r13,%r13
	jz	2f
	sub	$32,%r13
	cmp	(%r14,%r13),%r15
	ja	2f
	mov	8(%r14,%r13),%rax		# garbage.p[𝑖].fn
	mov	16(%r14,%r13),%rdi		# garbage.p[𝑖].arg
	call	*%rax
	decq	(%r12)
	jmp	1b
2:	pop	%rsi
	pop	%rdi
	jmp	0b
	.endfn	gclongjmp,globl
	.source	__FILE__
