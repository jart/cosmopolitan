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
.source	__FILE__
.privileged
.alignfunc

/	Returns 𝑥*𝑦, aborting on overflow.
/
/	@param	rdi:rsi is int128 𝑥
/	@param	rdx:rcx is int128 𝑦
/	@return	rdx:rax is 𝑥*𝑦
/	@see	-ftrapv
__mulvti3:
	push	%rbp
	mov	%rsp,%rbp
	push	%rbx
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15
	mov	%rdx,%r10
	mov	%rdi,%rdx
	xor	%r11d,%r11d
	mov	%r10,%rax
	sar	$63,%rdx
	sar	$63,%rax
	cmp	%rsi,%rdx
	jne	4f
	cmp	%rcx,%rax
	jne	5f
	mov	%rdi,%rax
	imul	%r10
	mov	%rax,%r14
	mov	%rdx,%r8
	jmp	2f
5:	mov	%r10,%r12
	mov	%rcx,%r13
	mov	%rcx,%r8
	mov	%rdi,%rbx
	jmp	6f
4:	cmp	%rcx,%rax
	jne	7f
	mov	%rdi,%r12
	mov	%rsi,%r13
	mov	%rsi,%r8
	mov	%r10,%rbx
6:	mov	%rdi,%rax
	mul	%r10
	mov	%rax,%r14
	mov	%rbx,%rax
	mov	%rdx,%r15
	mul	%r8
	test	%r8,%r8
	jns	8f
	xor	%r8d,%r8d
	sub	%r8,%rax
	sbb	%rbx,%rdx
8:	test	%rbx,%rbx
	jns	9f
	sub	%r12,%rax
	sbb	%r13,%rdx
9:	mov	%r15,%r8
	xor	%r9d,%r9d
	add	%rax,%r8
	adc	%rdx,%r9
	mov	%r8,%rdx
	sar	$63,%rdx
	cmp	%r9,%rdx
	je	2f
	imul	%r10,%rsi
	mov	%rdi,%rax
	imul	%rdi,%rcx
	mul	%r10
	lea	(%rsi,%rcx),%r8
	add	%rdx,%r8
	mov	%rax,%r14
	jmp	3f
7:	mov	%rsi,%r8
	mov	%rcx,%rdx
	mov	%rdi,%rax
	imul	%rdi,%rdx
	imul	%r10,%r8
	add	%rdx,%r8
	mul	%r10
	mov	%rax,%r14
	lea	1(%rsi),%rax
	add	%rdx,%r8
	cmp	$1,%rax
	ja	3f
	lea	1(%rcx),%rax
	cmp	$1,%rax
	ja	3f
	cmp	%rcx,%rsi
	jne	11f
	cmp	%r14,%r11
	mov	%r11,%rax
	sbb	%r8,%rax
	jl	2f
	jmp	3f
11:	test	%r8,%r8
	js	2f
3:	mov	$1,%r11d
2:	test	%r11,%r11
	je	1f
	mov	%r8,-8(%rbp)
	call	__on_arithmetic_overflow
	mov	-8(%rbp),%r8
1:	mov	%r14,%rax
	mov	%r8,%rdx
	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
	leave
	ret
	.endfn	__mulvti3,globl
