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
#include "ape/lib/pc.h"
#include "ape/config.h"
#include "ape/macros.h"
#include "ape/notice.inc"
.section .real,"ax",@progbits
.source	__FILE__
.code16

	.globl	e820map
	.hidden	e820map
	.type	e820map,@object
	.size	e820map,XLM_E820_SIZE
	e820map = ape.xlm + XLM_E820

	.globl	e820map_xlm
	.hidden	e820map_xlm
	.type	e820map_xlm,@object
	.size	e820map_xlm,XLM_E820_SIZE
	e820map_xlm = XLM(E820)
