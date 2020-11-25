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
/	@param	𝑥 is double scalar in low half of %xmm0
/	@return	double scalar in low half of %xmm0
/	@define	round(𝑥) = copysign(trunc(fabs(𝑥)+.5),𝑥)
/		round(𝑥) = trunc(𝑥+copysign(.5,𝑥))
tinymath_round:
#if !X86_NEED(SSE4_2)
	testb	X86_HAVE(SSE4_2)+kCpuids(%rip)
	jz	tinymath_round$k8
	.text.antiquity
tinymath_round$k8:
	.leafprologue
	.profilable
	movapd	%xmm0,%xmm1
	movsd	D(%rip),%xmm2
	movsd	C(%rip),%xmm3
	andpd	%xmm2,%xmm1
	ucomisd	%xmm1,%xmm3
	jbe	2f
	addsd	A(%rip),%xmm1
	andnpd	%xmm0,%xmm2
	movapd	%xmm2,%xmm0
	cvttsd2siq %xmm1,%rax
	pxor	%xmm1,%xmm1
	cvtsi2sdq %rax,%xmm1
	orpd	%xmm1,%xmm0
2:	.leafepilogue
	.endfn	tinymath_round$k8,globl,hidden
	.previous
	.rodata.cst16
C:	.quad	0x4330000000000000,0
D:	.quad	0x7fffffffffffffff,0
	.previous
#endif
	movapd	%xmm0,%xmm1
	andpd	B(%rip),%xmm0
	orpd	A(%rip),%xmm0
	addsd	%xmm1,%xmm0
	roundsd $_MM_FROUND_TO_ZERO,%xmm0,%xmm0
	ret
	.endfn	tinymath_round,globl
	.alias	tinymath_round,round

	.rodata.cst16
A:	.quad	0x3fdfffffffffffff,0
B:	.quad	0x8000000000000000,0
