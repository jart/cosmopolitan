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
.source	__FILE__

tinymath_cabsf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$16,%rsp
	movq	%xmm0,(%rsp)
	movss	(%rsp),%xmm0
	movss	4(%rsp),%xmm2
	movaps	%xmm0,%xmm1
	mulss	%xmm2,%xmm2
	mulss	%xmm0,%xmm1
	movaps	%xmm2,%xmm0
	addss	%xmm1,%xmm0
	sqrtss	%xmm0,%xmm0
	leave
	ret
	.endfn	tinymath_cabsf,globl
	.alias	tinymath_cabsf,cabsf
