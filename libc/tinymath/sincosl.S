/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "ape/lib/pc.h"
#include "libc/macros.h"
.source	__FILE__

/	Returns sine and cosine of 𝑥.
/
/	@param	𝑥 is an 80-bit long double passed on stack in 16-bytes
/	@param	%rdi is long double *out_sin
/	@param	%rsi is long double *out_cos
/	@domain	-(3π/8) < 𝑥 < 3π/8 for best accuracy
tinymath_sincosl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	fldt	16(%rbp)
0:	fsincos
	fstsw
	test	$FPU_C2>>8,%ah
	jnz	1f
	fstpt	(%rsi)
	fstpt	(%rdi)
	pop	%rbp
	ret
1:	fldpi
	fadd	%st
	fxch
2:	fprem1
	fnstsw
	test	$FPU_C2>>8,%ah
	jnz	2b
	fstp	%st(1)
	jmp	0b
	.endfn	tinymath_sincosl,globl
	.alias	tinymath_sincosl,sincosl
