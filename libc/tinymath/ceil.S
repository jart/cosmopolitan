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
.source	__FILE__

tinymath_ceil:
	.leafprologue
	.profilable
	movsd	.Llol(%rip),%xmm1
	movsd	.Lcat(%rip),%xmm2
	andpd	%xmm0,%xmm1
	comisd	%xmm1,%xmm2
	jbe	1f
	cvttsd2siq %xmm0,%rax
	pxor	%xmm1,%xmm1
	movsd	.Lmog(%rip),%xmm2
	cvtsi2sdq %rax,%xmm1
	cmpnlesd %xmm1,%xmm0
	andpd	%xmm2,%xmm0
	addsd	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_ceil,globl
	.alias	tinymath_ceil,ceil

	.rodata.cst16
.Llol:	.long	4294967295
	.long	2147483647
	.long	0
	.long	0

	.rodata.cst8
.Lcat:	.long	0
	.long	1127219200
.Lmog:	.long	0
	.long	1072693248

/	vroundsd $_MM_FROUND_TO_POS_INF|_MM_FROUND_NO_EXC,%xmm0,%xmm0,%xmm0
