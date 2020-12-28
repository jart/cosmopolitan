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
#include "libc/bits/smmintrin.internal.h"
#include "libc/nexgen32e/x86feature.h"
.source	__FILE__

/	Rounds to nearest integer, away from zero.
/
/	@param	𝑥 is float scalar in low quarter of %xmm0
/	@return	float scalar in low quarter of %xmm0
tinymath_roundf:
#if !X86_NEED(SSE4_2)
	testb	X86_HAVE(SSE4_2)+kCpuids(%rip)
	jz	tinymath_roundf$k8
	.text.antiquity
tinymath_roundf$k8:
	.leafprologue
	.profilable
	movaps	%xmm0,%xmm1
	movss	D(%rip),%xmm2
	movss	C(%rip),%xmm3
	andps	%xmm2,%xmm1
	ucomiss	%xmm1,%xmm3
	jbe	2f
	addss	A(%rip),%xmm1
	cvttss2sil %xmm1,%eax
	pxor	%xmm1,%xmm1
	cvtsi2ssl %eax,%xmm1
	andnps	%xmm0,%xmm2
	movaps	%xmm2,%xmm0
	orps	%xmm1,%xmm0
2:	.leafepilogue
	.endfn	tinymath_roundf$k8,globl,hidden
	.previous
	.rodata.cst16
C:	.long	0x4b000000,0,0,0
D:	.long	0x7fffffff,0,0,0
	.previous
#endif
	movaps	%xmm0,%xmm1
	andps	B(%rip),%xmm0
	orps	A(%rip),%xmm0
	addss	%xmm1,%xmm0
	roundss $_MM_FROUND_TO_ZERO,%xmm0,%xmm0
	ret
	.endfn	tinymath_roundf,globl
	.alias	tinymath_roundf,roundf

	.rodata.cst16
A:	.long	0x3effffff,0,0,0
B:	.long	0x80000000,0,0,0
