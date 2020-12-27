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

tinymath_copysign:
	.leafprologue
	.profilable
	movapd	%xmm1,%xmm2
	andpd	.Lnan(%rip),%xmm0
	andpd	.Lneg0(%rip),%xmm2
	orpd	%xmm2,%xmm0
	.leafepilogue
	.endfn	tinymath_copysign,globl
	.alias	tinymath_copysign,copysign

	.rodata.cst16
.Lnan:	.long	0xffffffff
	.long	0x7fffffff
	.long	0
	.long	0
.Lneg0:	.long	0
	.long	-2147483648
	.long	0
	.long	0
