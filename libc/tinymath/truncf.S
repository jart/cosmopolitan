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

tinymath_truncf:
	.leafprologue
	.profilable
	movss	3f(%rip),%xmm1
	andps	%xmm0,%xmm1
	movss	2f(%rip),%xmm2
	comiss	%xmm1,%xmm2
	jbe	1f
	cvttss2si %xmm0,%eax
	pxor	%xmm0,%xmm0
	cvtsi2ss %eax,%xmm0
1:	.leafepilogue
	.endfn	tinymath_truncf,globl
	.alias	tinymath_truncf,truncf

	.rodata.cst4
2:	.long	0x4b000000
	.rodata.cst16
3:	.long	0x7fffffff,0,0,0

/	TODO(jart)
/	roundss $_MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC,%xmm0,%xmm0
