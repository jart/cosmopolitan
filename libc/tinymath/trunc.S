/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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
.yoink	__FILE__

/	Rounds to integer, toward zero.
/
/	@param	𝑥 is double scalar in low half of %xmm0
/	@return	double scalar in low half of %xmm0
/	@define	trunc(𝑥+copysign(.5,𝑥))
/	@see	round(),rint(),nearbyint()
/	@see	roundsd $_MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC,%xmm0,%xmm0
tinymath_trunc:
	.leafprologue
	.profilable
	movsd	3f(%rip),%xmm1
	movsd	2f(%rip),%xmm2
	andpd	%xmm0,%xmm1
	comisd	%xmm1,%xmm2
	jbe	1f
	cvttsd2siq %xmm0,%rax
	pxor	%xmm0,%xmm0
	cvtsi2sdq %rax,%xmm0
1:	.leafepilogue
	.endfn	tinymath_trunc,globl
	.alias	tinymath_trunc,trunc

	.rodata.cst8
2:	.long	0
	.long	1127219200

	.rodata.cst16
3:	.long	4294967295
	.long	2147483647
	.long	0
	.long	0
