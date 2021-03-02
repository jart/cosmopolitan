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

//	Returns smallest integral not less than 𝑥.
//
//	@param	𝑥 is double scalar in low half of %xmm0
//	@return	double scalar in low half of %xmm0
//	@see	round(),rint(),nearbyint()
//	@see vroundsd $_MM_FROUND_TO_POS_INF|_MM_FROUND_NO_EXC,%xmm0,%xmm0,%xmm0
ceil:	.leafprologue
	.profilable
	movsd	4f(%rip),%xmm3
	movsd	2f(%rip),%xmm4
	movapd	%xmm0,%xmm2
	movapd	%xmm0,%xmm1
	andpd	%xmm3,%xmm2
	ucomisd	%xmm2,%xmm4
	jbe	1f
	cvttsd2siq %xmm0,%rax
	pxor	%xmm2,%xmm2
	movsd	3f(%rip),%xmm4
	andnpd	%xmm1,%xmm3
	cvtsi2sdq %rax,%xmm2
	cmpnlesd %xmm2,%xmm0
	andpd	%xmm4,%xmm0
	addsd	%xmm2,%xmm0
	orpd	%xmm3,%xmm0
1:	.leafepilogue
	.endfn	ceil,globl

	.rodata.cst8
2:	.long	0x00000000
	.long	0x43300000
3:	.long	0x00000000
	.long	0x3ff00000
	.rodata.cst16
4:	.long	0xffffffff
	.long	0x7fffffff
	.long	0x00000000
	.long	0x00000000
