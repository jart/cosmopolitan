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

tinymath_ceilf:
	.leafprologue
	.profilable
	movss	.L3(%rip),%xmm1
	andps	%xmm0,%xmm1
	movss	.L2(%rip),%xmm2
	comiss	%xmm1,%xmm2
	jbe	1f
	cvttss2si %xmm0,%eax
	pxor	%xmm1,%xmm1
	movss	.L1(%rip),%xmm2
	cvtsi2ss %eax,%xmm1
	cmpnless %xmm1,%xmm0
	andps	%xmm2,%xmm0
	addss	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_ceilf,globl
	.alias	tinymath_ceilf,ceilf

	.rodata.cst4
.L1:	.float	1.0
.L2:	.long	1258291200

	.rodata.cst16
.L3:	.long	2147483647
	.long	0
	.long	0
	.long	0

/	TODO(jart):
/	vroundss	$10,%xmm0,%xmm0,%xmm0
