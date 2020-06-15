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

/	Verifies buffer contains only URI characters.
/
/	@param	%rdi is data which should be 32-byte aligned
/	@param	%rsi is byte length of data
/	@return	number of kosher bytes
/	@cost	10x faster than fastest Ragel code
uricspn$avx:
	.leafprologue
	.profilable
	vmovaps	.Luric(%rip),%xmm0
	mov	$14,%eax
	mov	%rsi,%rdx
	xor	%esi,%esi
0:	vmovdqu	(%rdi,%rsi),%xmm1
	vmovdqu	16(%rdi,%rsi),%xmm2
	vpcmpestri $0b00010100,%xmm1,%xmm0
	jc	1f
	jo	1f
	add	$16,%rsi
	sub	$16,%rdx
	vpcmpestri $0b00010100,%xmm2,%xmm0
	jc	1f
	jo	1f
	add	$16,%rsi
	sub	$16,%rdx
	jmp	0b
1:	lea	(%rsi,%rcx),%rax
	.leafepilogue
	.endfn	uricspn$avx,globl,hidden

	.rodata.cst16
.Luric:	.byte	'!,'!
	.byte	'$,';
	.byte	'=,'=
	.byte	'?,'Z
	.byte	'_,'_
	.byte	'a,'z
	.byte	'~,'~
	.byte	0,0
	.endobj	.Luric
	.previous
