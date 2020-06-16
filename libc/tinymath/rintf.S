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
.source	__FILE__

tinymath_rintf:
	.leafprologue
	.profilable
	movaps	%xmm0,%xmm1
	movss	.LC8(%rip),%xmm2
	andps	%xmm2,%xmm1
	movss	.LC7(%rip),%xmm3
	comiss	%xmm1,%xmm3
	jbe	1f
	addss	%xmm3,%xmm1
	andnps	%xmm0,%xmm2
	movaps	%xmm2,%xmm0
	subss	%xmm3,%xmm1
	orps	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_rintf,globl
	.alias	tinymath_rintf,rintf

	.rodata.cst4
.LC7:	.long	1258291200

	.rodata.cst16
.LC8:	.long	2147483647
	.long	0
	.long	0
	.long	0

/	TODO(jart):
/	vroundss	$4,%xmm0,%xmm0,%xmm0
