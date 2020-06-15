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
/* clang-format off */
.include "libc/macros.inc"

.macro	.syscon	group:req name:req linux:req xnu:req freebsd:req openbsd:req windows:req
	yoink	_init_systemfive
 .section .piro.bss.sort.syscon.2.\group\().\name,"aw",@nobits
\name:	.quad	0
	.endobj	\name,globl,hidden
 .previous
 .section .sort.rodata.syscon.linux.2.\group\().\name,"a",@progbits
	.sleb128 \linux
 .previous
 .section .sort.rodata.syscon.xnu.2.\group\().\name,"a",@progbits
	.sleb128 \xnu
 .previous
 .section .sort.rodata.syscon.freebsd.2.\group\().\name,"a",@progbits
	.sleb128 \freebsd
 .previous
 .section .sort.rodata.syscon.openbsd.2.\group\().\name,"a",@progbits
	.sleb128 \openbsd
 .previous
 .section .sort.rodata.syscon.windows.2.\group\().\name,"a",@progbits
	.sleb128 \windows
 .previous
.endm
