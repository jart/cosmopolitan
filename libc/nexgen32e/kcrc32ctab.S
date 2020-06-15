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

/	Castagnoli CRC32 ISCSI Polynomial
/	x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1
/	0b00011110110111000110111101000001
/	bitreverse32(0x1edc6f41)
#define ISCSI_POLYNOMIAL 0x82f63b78

	.initbss 300,_init_kCrc32cTab
kCrc32cTab:
	.rept	256
	.long	0
	.endr
	.endobj	kCrc32cTab,globl,hidden
	.previous

	.init.start 300,_init_kCrc32cTab
	push	%rsi
	mov	$ISCSI_POLYNOMIAL,%esi
	call	crc32init
	pop	%rsi
	.init.end 300,_init_kCrc32cTab
	.yoink	__FILE__
