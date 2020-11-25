/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
