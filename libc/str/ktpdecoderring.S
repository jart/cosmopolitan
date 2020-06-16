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

/	Thompson-Pike Decoder Ring.
/
/	The IA-32 BSR instruction can be used to turn a 32-bit
/	number into an index for this table.
/
/	@see	libc/str/internal.h
	.rodata
	.align	2
kTpDecoderRing:
	.rept	7			# MSB≤6 (0x7F)
	.byte	0b00000000,1		# mark,len
/		0b11000000		# mask
	.endr
	.rept	4			# MSB≤10 (0x7FF)
	.byte	0b11000000,2		# mark,len
/		0b11100000		# mask
	.endr
	.rept	5			# MSB≤15 (0xFFFF)
	.byte	0b11100000,3		# mark,len
/		0b11110000		# mask
	.endr
	.rept	5			# MSB≤20 (0x1FFFFF)
	.byte	0b11110000,4		# mark,len
/		0b11111000		# mask
	.endr
	.rept	5			# MSB≤25 (0x3FFFFFF)
	.byte	0b11111000,5		# mark,len
/		0b11111100		# mask
	.endr
	.rept	6			# MSB≤31 (0xffffffff)
	.byte	0b11111100,6		# mark,len
	.endr
	.endobj	kTpDecoderRing,globl,hidden
	.previous
