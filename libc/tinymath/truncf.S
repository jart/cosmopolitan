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
#include "libc/macros.internal.h"

//	Rounds to integer, toward zero.
//
//	@param	𝑥 is float scalar in low quarter of %xmm0
//	@return	float scalar in low quarter of %xmm0
//	@define	trunc(𝑥+copysign(.5,𝑥))
//	@see	round(),rint(),nearbyint()
//	@see	roundss $_MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC,%xmm0,%xmm0
truncf:	.leafprologue
	.profilable
	movss	3f(%rip),%xmm2
	movss	2f(%rip),%xmm4
	movaps	%xmm0,%xmm3
	movaps	%xmm0,%xmm1
	andps	%xmm2,%xmm3
	ucomiss	%xmm3,%xmm4
	jbe	1f
	cvttss2sil	%xmm0,%eax
	pxor	%xmm0,%xmm0
	andnps	%xmm1,%xmm2
	cvtsi2ssl	%eax,%xmm0
	orps	%xmm2,%xmm0
1:	.leafepilogue
	.endfn	truncf,globl

	.rodata.cst4
2:	.long	0x4b000000
	.rodata.cst16
3:	.long	0x7fffffff,0,0,0
