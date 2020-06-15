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

tinymath_ilogbl:
	.profilable
	sub	$24,%rsp
	fldt	32(%rsp)
	fnstcw	14(%rsp)
	movzwl	14(%rsp),%eax
	orb	$12,%ah
	movw	%ax,12(%rsp)
	fxtract
	fstp	%st
	fldcw	12(%rsp)
	fistpl	8(%rsp)
	fldcw	14(%rsp)
	movl	8(%rsp),%eax
	add	$24,%rsp
	ret
	.endfn	tinymath_ilogbl,globl
	.alias	tinymath_ilogbl,ilogbl

/*
	TODO(jart)
.globl	ilogbl
.type	ilogbl,@function
ilogbl:	sub	$24,%rsp
	fldt	32(%rsp)
	fxtract
	fstp	%st
	fisttpl	12(%rsp)
	movl	12(%rsp),%eax
	add	$24,%rsp
	ret
.size	ilogbl,.-ilogbl
*/
