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

/	Returns binary logarithm of integer 𝑥.
/
/	@param	rsi:rdi is 128-bit unsigned 𝑥 value
/	@return	eax number in range [0,128) or undef if 𝑥 is 0
/	@see	also treasure trove of nearly identical functions
/
/	      uint32 𝑥   bsf(𝑥) tzcnt(𝑥)   ffs(𝑥)   bsr(𝑥) lzcnt(𝑥)
/	    0x00000000      wut       32        0      wut       32
/	    0x00000001        0        0        1        0       31
/	    0x80000001        0        0        1       31        0
/	    0x80000000       31       31       32       31        0
/	    0x00000010        4        4        5        4       27
/	    0x08000010        4        4        5       27        4
/	    0x08000000       27       27       28       27        4
/	    0xffffffff        0        0        1       31        0
/
bsrmax:	.leafprologue
	.profilable
	bsr	%rsi,%rax
	jnz	2f
	bsr	%rdi,%rax
1:	.leafepilogue
2:	add	$64,%eax
	jmp	1b
	.endfn	bsrmax,globl
	.yoink	__FILE__
