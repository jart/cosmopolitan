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

/	Returns log(𝟷+𝑥).
/
/	@param	𝑥 is double scalar in low half of %xmm0
/	@return	double scalar in low half of %xmm0
tinymath_log1p:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%rax
	vmovsd	%xmm0,(%rsp)
	fldl	(%rsp)
	fld	%st
	fabs
	fldt	.LC16(%rip)
	fxch
	fcomip	%st(1),%st
	fstp	%st
	jnb	1f
	fldln2
	fxch
	fyl2xp1
	fstpl	(%rsp)
	vmovsd	(%rsp),%xmm0
0:	leave
	ret
1:	fld1
	faddp	%st,%st(1)
	fldln2
	fxch
	fyl2x
	fstpl	(%rsp)
	vmovsd	(%rsp),%xmm0
	jmp	0b
	.endfn	tinymath_log1p,globl
	.alias	tinymath_log1p,log1p

	.rodata.cst16
.LC16:	.long	205731576
	.long	2515933592
	.long	16381
	.long	0
