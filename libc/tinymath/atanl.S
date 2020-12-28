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
#include "libc/macros.h"
.source	__FILE__

/	Returns arc tangent of 𝑥.
/
/	                  1  3   1  5   1  7   1  9    1  11
/	    atan(𝑥) = 𝑥 - - 𝑥  + - 𝑥  - - 𝑥  + - 𝑥  - -- 𝑥   ...
/	                  3      5      7      9      11
/
/	@param	𝑥 is an 80-bit long double passed on stack in 16-bytes
/	@return	result of computation on FPU stack in %st
/	@define	atan(𝑥) = Σₙ₌₀₋∞ 2²ⁿ(𝑛!)²/(𝟸𝑛+𝟷)!(𝑥²ⁿ⁺¹/(𝑥²+𝟷)ⁿ⁺¹)
tinymath_atanl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	fldt	16(%rbp)
	fld1
	fpatan
	pop	%rbp
	ret
	.endfn	tinymath_atanl,globl
	.alias	tinymath_atanl,atanl
