/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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
#ifndef COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_
#define COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_
#include "ape/relocations.h"
#include "libc/macros.h"
#ifdef __ASSEMBLER__
/* clang-format off */

.macro	.ntimp	fn:req
	yoink	_init_ntdll
	.initbss 202,_init_ntdll.\fn
__imp_\fn:
	.quad	0
	.endobj	__imp_\fn,globl,hidden
	.previous
	.initro 202,_init_ntdll.2.\fn
	.quad	RVA(.L\fn)
	.previous
	.section .rodata.str1.1,"aSM",@progbits,1
.L\fn:
	.asciz	"\fn"
	.previous
.endm

/* clang-format on */
#endif /* __ASSEMBLER__ */
#endif /* COSMOPOLITAN_LIBC_NT_NTDLLIMPORT_H_ */
