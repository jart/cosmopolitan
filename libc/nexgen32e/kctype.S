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
#include "libc/notice.inc"

	.initbss 300,_init_kCtype
kCtype:	.rept	256
	.byte	0
	.endr
	.endobj	kCtype,globl,hidden
	.previous

	.initro 300,_init_kCtype
.LkCtype.rodata:
/	                  ┌─isblank
/	                  │┌─isupper
/	                  ││┌─islower
/	                  │││┌─isprint
/	                  ││││┌─isxdigit
/	                  │││││┌─isdigit
/	                  ││││││┌─isalpha
/	                  │││││││┌─isspace
/	                  ││││││││
	.byte	9,	0b00000000	# 00─08 ∅─◘
	.byte	1,	0b10000001	# 09─09 ○
	.byte	4,	0b00000001	# 0a─0d ◙─♪
	.byte	18,	0b00000000	# 0e─1f ♫─▼
	.byte	1,	0b10010001	# 20─20
	.byte	15,	0b00010000	# 21─2f !─/
	.byte	10,	0b00011100	# 30─39 0─9
	.byte	7,	0b00010000	# 3a─40 :─@
	.byte	6,	0b01011010	# 41─46 A─F
	.byte	20,	0b01010010	# 47─5a G─Z
	.byte	6,	0b00010000	# 5b─60 [─`
	.byte	6,	0b00111010	# 61─66 a─f
	.byte	20,	0b00110010	# 67─7a g─z
	.byte	4,	0b00010000	# 7b─7e {─~
	.byte	129,	0b00000000	# 7f─ff ⌂─λ
	.endobj	.LkCtype.rodata		# 32b ─ 13%
	.byte	0,0			# terminatr
	.previous

	.init.start 300,_init_kCtype
	call	rldecode
	.init.end 300,_init_kCtype
	.source	__FILE__
