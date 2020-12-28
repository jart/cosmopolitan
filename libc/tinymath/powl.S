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

/	Returns 𝑥^𝑦.
/
/	@param	𝑥 is an 80-bit long double passed on stack in 16-bytes
/	@param	𝑦 is the power, also pushed on stack, in reverse order
/	@return	result of exponentiation on FPU stack in %st
/	@note	Sun's fdlibm needs 2kLOC to do this for RISC lool
tinymath_powl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	fldt	32(%rbp)
	fldt	16(%rbp)
	fyl2x
	fld1
	fld	%st(1)
	fprem
	f2xm1
	faddp
	fscale
	fxch
	fstp	%st
	pop	%rbp
	ret
	.endfn	tinymath_powl,globl
	.alias	tinymath_powl,powl
	.alias	tinymath_powl,__powl_finite
