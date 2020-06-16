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

tinymath_floorf:
	.leafprologue
	.profilable
	movss	.LC8(%rip),%xmm1
	andps	%xmm0,%xmm1
	movss	.LC7(%rip),%xmm2
	comiss	%xmm1,%xmm2
	jbe	1f
	cvttss2si %xmm0,%eax
	pxor	%xmm1,%xmm1
	movss	.LC3(%rip),%xmm2
	cvtsi2ss %eax,%xmm1
	movaps	%xmm1,%xmm3
	cmpnless %xmm0,%xmm3
	movaps	%xmm3,%xmm0
	andps	%xmm2,%xmm0
	subss	%xmm0,%xmm1
	movaps	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_floorf,globl
	.alias	tinymath_floorf,floorf

	.rodata.cst4
.LC3:	.float	1.0
.LC7:	.long	0x4b000000

	.rodata.cst16
.LC8:	.long	2147483647,0,0,0
