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

tinymath_cprojl:
	.profilable
	sub	$24,%rsp
	fldt	32(%rsp)
	fnstcw	14(%rsp)
	movzwl	14(%rsp),%eax
	orb	$12,%ah
	movw	%ax,12(%rsp)
	fldcw	12(%rsp)
	fistpq	(%rsp)
	fldcw	14(%rsp)
	movq	(%rsp),%rsi
	fldt	48(%rsp)
	mov	%rsi,%rax
	fldcw	12(%rsp)
	fistpq	(%rsp)
	fldcw	14(%rsp)
	movq	(%rsp),%rcx
	add	$24,%rsp
	mov	%rcx,%rdx
	ret
	.endfn	tinymath_cprojl,globl
	.alias	tinymath_cprojl,cprojl

/	TODO(jart):
/	sub	$24,%rsp
/	fldt	32(%rsp)
/	fisttpq	8(%rsp)
/	fldt	48(%rsp)
/	movq	8(%rsp),%rsi
/	mov	%rsi,%rax
/	fisttpq	8(%rsp)
/	movq	8(%rsp),%rcx
/	add	$24,%rsp
/	mov	%rcx,%rdx
/	ret
