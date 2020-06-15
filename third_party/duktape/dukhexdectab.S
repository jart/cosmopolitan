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

	.initbss 300,_init_duk_hex_dectab
duk_hex_dectab:
	.zero	256
	.endobj	duk_hex_dectab,globl,hidden
	.previous

	.initro 300,_init_duk_hex_dectab
.Lduk_hex_dectab.rodata:	# 56 bytes (22%)
	.byte	48,0xff		# 00-2f ∅-/
	.byte	1,0x00		# 30-30 0
	.byte	1,0x01		# 31-31 1
	.byte	1,0x02		# 32-32 2
	.byte	1,0x03		# 33-33 3
	.byte	1,0x04		# 34-34 4
	.byte	1,0x05		# 35-35 5
	.byte	1,0x06		# 36-36 6
	.byte	1,0x07		# 37-37 7
	.byte	1,0x08		# 38-38 8
	.byte	1,0x09		# 39-39 9
	.byte	7,0xff		# 3a-40 :-@
	.byte	1,0x0a		# 41-41 A
	.byte	1,0x0b		# 42-42 B
	.byte	1,0x0c		# 43-43 C
	.byte	1,0x0d		# 44-44 D
	.byte	1,0x0e		# 45-45 E
	.byte	1,0x0f		# 46-46 F
	.byte	26,0xff		# 47-60 G-`
	.byte	1,0x0a		# 61-61 a
	.byte	1,0x0b		# 62-62 b
	.byte	1,0x0c		# 63-63 c
	.byte	1,0x0d		# 64-64 d
	.byte	1,0x0e		# 65-65 e
	.byte	1,0x0f		# 66-66 f
	.byte	153,0xff	# 67-ff g-λ
	.endobj	.Lduk_hex_dectab.rodata
	.byte	0,0		# terminator
	.byte	0,0		# padding
	.previous

	.init.start 300,_init_duk_hex_dectab
	call	rldecode
	lodsw
	.init.end 300,_init_duk_hex_dectab
