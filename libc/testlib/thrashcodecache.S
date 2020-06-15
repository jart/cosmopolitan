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
.testonly
.yoink	__FILE__

/	Empties L1 instruction cache.
thrashcodecache:
	.leafprologue
	push	%rbx
	xor	%eax,%eax
	xor	%ecx,%ecx
	cpuid
/	pushpop	4,%rcx
0:	.rept	32768/(8+9)
	.byte	0x0f,0x1f,0x84,0,0,0,0,0		# fat nop x8
	.byte	0x66,0x0f,0x1f,0x84,0,0,0,0,0		# fat nop x9
	.endr
/	loop	0b
	xor	%eax,%eax
	xor	%ecx,%ecx
	cpuid
	pop	%rbx
	.leafepilogue
	.endfn	thrashcodecache,globl
