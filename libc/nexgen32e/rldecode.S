/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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
.text.startup

/	Seventeen byte decompressor.
/
/	@param	di points to output buffer
/	@param	si points to uint8_t {len₁,byte₁}, ..., {0,0}
/	@mode	long,legacy,real
rldecode:
	.leafprologue
	.profilable
	xor	%ecx,%ecx
0:	lodsb
	xchg	%al,%cl
	lodsb
	jrcxz	2f
1:	stosb
	loop	1b
	jmp	0b
2:	.leafepilogue
	.endfn	rldecode,globl
	.source	__FILE__
