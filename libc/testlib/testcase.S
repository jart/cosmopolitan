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
#include "libc/macros.h"
.source	__FILE__

/	Decentralized section for test testcase registration.
/
/	@see	ape/ape.lds
	.section .piro.relo.sort.testcase.1,"aw",@nobits
	.type	__testcase_start,@object
	.type	__testcase_end,@object
	.globl	__testcase_start,__testcase_end
	.hidden	__testcase_start,__testcase_end
	.byte	0
	.align	__SIZEOF_POINTER__
__testcase_start:
	.previous/*
	...
	decentralized content
	...
	*/.section .piro.relo.sort.testcase.3,"aw",@nobits
__testcase_end:
	.quad	0
	.previous
