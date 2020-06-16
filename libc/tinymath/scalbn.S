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

tinymath_scalbn:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%rax
	movsd	%xmm0,-8(%rbp)
	fldl	-8(%rbp)
	movl	%edi,-8(%rbp)
	fildl	-8(%rbp)
	fxch	%st(1)
	fscale
	fstp	%st(1)
	fstpl	-8(%rbp)
	movsd	-8(%rbp),%xmm0
	leave
	ret
	.endfn	tinymath_scalbn,globl
	.alias	tinymath_scalbn,scalbn
