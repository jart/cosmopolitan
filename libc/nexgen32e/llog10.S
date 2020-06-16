/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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

/	Fast log₁₀ when 𝑥 is an integer.
/
/	@param	rdi is uint64 𝑥
/	@domain	0<𝑥<2⁶⁴ ∧ 𝑥∊ℤ
llog10:	.leafprologue
	.profilable
	bsr	%rdi,%rax
	jz	3f
	lea	llog10data(%rip),%rdx
	movsbl	1(%rdx,%rax),%eax
	cmp	2f-1f(%rdx,%rax,8),%rdi
	sbb	$0,%al
	jmp	4f
3:	xor	%eax,%eax	# domain error
4:	.leafepilogue
	.endfn	llog10,globl

	.rodata
llog10data:
1:	.byte	 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4
	.byte	 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9
	.byte	 9, 9,10,10,10,11,11,11,12,12,12,12,13,13,13,14
	.byte	14,14,15,15,15,15,16,16,16,17,17,17,18,18,18,18
	.byte	19,19,19
	.align	8
2:	.quad	0
	.quad	10
	.quad	100
	.quad	1000
	.quad	10000
	.quad	100000
	.quad	1000000
	.quad	10000000
	.quad	100000000
	.quad	1000000000
	.quad	10000000000
	.quad	100000000000
	.quad	1000000000000
	.quad	10000000000000
	.quad	100000000000000
	.quad	1000000000000000
	.quad	10000000000000000
	.quad	100000000000000000
	.endobj	llog10data
	.previous
	.source	__FILE__
