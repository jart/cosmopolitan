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

/	Rounds to nearest integer, away from zero.
/
/	@param	𝑥 is float scalar in low quarter of %xmm0
/	@return	64-bit signed integer in %rax
/	@see	round()
tinymath_lroundf:
	.leafprologue
	.profilable
	movss	A(%rip),%xmm1
	movss	B(%rip),%xmm2
	andps	%xmm0,%xmm2
	orps	%xmm2,%xmm1
	addss	%xmm0,%xmm1
	cvttss2siq %xmm1,%rax
	.leafepilogue
	.endfn	tinymath_lroundf,globl
	.alias	tinymath_lroundf,lroundf
	.alias	tinymath_lroundf,llroundf

	.rodata.cst4
A:	.long	0x3effffff
	.rodata.cst16
B:	.long	0x80000000,0,0,0
