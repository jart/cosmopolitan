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

tinymath_frexp:
	.leafprologue
	push	%rbx
	push	%rdx
	mov	%rdi,%rbx
	movq	%xmm0,%rax
	shr	$52,%rax
	and	$0x7ff,%eax
	jne	3f
	xorps	%xmm1,%xmm1
	ucomisd	%xmm1,%xmm0
	jp	1f
	je	2f
1:	mulsd	6f(%rip),%xmm0
	mov	%rbx,%rdi
	call	frexp
	subl	$64,(%rbx)
	jmp	5f
2:	movl	$0,(%rdi)
	jmp	5f
3:	cmp	$0x7ff,%eax
	je	5f
	movq	%xmm0,%rdx
	sub	$0x3fe,%eax
	mov	%eax,(%rdi)
	movabs	$-9218868437227405313,%rax
	and	%rax,%rdx
	mov	$511,%eax
	sal	$53,%rax
	or	%rax,%rdx
	movq	%rdx,%xmm0
5:	pop	%rax
	pop	%rbx
	.leafepilogue
	.endfn	tinymath_frexp,globl
	.alias	tinymath_frexp,frexp

	.rodata.cst8
6:	.long	0,0x43f00000
