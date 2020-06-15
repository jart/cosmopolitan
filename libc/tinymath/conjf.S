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
.yoink	__FILE__

tinymath_conjf:
	.profilable
	sub	$16,%rsp
	movq	%xmm0,8(%rsp)
	movss	12(%rsp),%xmm0
	xorps	.L1(%rip),%xmm0
	movss	8(%rsp),%xmm1
	movss	%xmm1,(%rsp)
	movss	%xmm0,4(%rsp)
	movq	(%rsp),%xmm0
	add	$16,%rsp
	ret
	.endfn	tinymath_conjf,globl
	.alias	tinymath_conjf,conjf

	.rodata.cst16
.L1:	.long	2147483648
	.long	0
	.long	0
	.long	0
