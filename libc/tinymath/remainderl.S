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
#include "libc/macros.h"
.source	__FILE__

tinymath_remainderl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	fldt	32(%rbp)
	fldt	16(%rbp)
1:	fprem1
	fnstsw	%ax
	test	$FPU_C2>>8,%ah
	jne	1b
	fstp	%st(1)
	pop	%rbp
	ret
	.endfn	tinymath_remainderl,globl
	.alias	tinymath_remainderl,remainderl
	.alias	tinymath_remainderl,dreml
