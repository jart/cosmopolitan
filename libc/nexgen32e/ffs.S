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

/	Finds lowest set bit in word.
/
/	@param	edi is the input number
/	@return	number in range [1,32] or 0 if no bits set
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
/	@asyncsignalsafe
ffs:	.leafprologue
	.profilable
	bsf	%edi,%eax
	or	$-1,%edx
	cmovz	%edx,%eax
	inc	%eax
	.leafepilogue
	.endfn	ffs,globl
	.yoink	__FILE__
