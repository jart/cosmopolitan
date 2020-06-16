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

/	Rounds to nearest integer, away from zero.
/
/	@param	𝑥 is double scalar in low half of %xmm0
/	@return	64-bit signed integer in %rax
/	@see	round(), lrint()
tinymath_lround:
	.leafprologue
	.profilable
	movsd	A(%rip),%xmm2
	movsd	B(%rip),%xmm1
	andpd	%xmm0,%xmm2
	orpd	%xmm2,%xmm1
	addsd	%xmm0,%xmm1
	cvttsd2siq %xmm1,%rax
	.leafepilogue
	.endfn	tinymath_lround,globl
	.alias	tinymath_lround,lround
	.alias	tinymath_lround,llround

	.rodata.cst16
A:	.quad	0x8000000000000000,0
B:	.quad	0x3fdfffffffffffff,0
