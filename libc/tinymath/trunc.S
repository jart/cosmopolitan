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
//	@param	𝑥 is double scalar in low half of %xmm0
//	@return	double scalar in low half of %xmm0
//	@define	trunc(𝑥+copysign(.5,𝑥))
//	@see	round(),rint(),nearbyint()
//	@see	roundsd $_MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC,%xmm0,%xmm0
trunc:	.leafprologue
	.profilable
	movsd	3f(%rip),%xmm2
	movsd	2f(%rip),%xmm4
	movapd	%xmm0,%xmm3
	movapd	%xmm0,%xmm1
	andpd	%xmm2,%xmm3
	ucomisd	%xmm3,%xmm4
	jbe	1f
	cvttsd2siq %xmm0,%rax
	pxor	%xmm0,%xmm0
	andnpd	%xmm1,%xmm2
	cvtsi2sdq %rax,%xmm0
	orpd	%xmm2,%xmm0
1:	.leafepilogue
	.endfn	trunc,globl

	.rodata.cst8
2:	.long	0x00000000
	.long	0x43300000
	.rodata.cst16
3:	.long	0xffffffff
	.long	0x7fffffff
	.long	0x00000000
	.long	0x00000000
