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
#include "ape/lib/pc.h"
#include "libc/macros.h"
.source	__FILE__

tinymath_lroundl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%rax
	push	%rax
	fldt	16(%rbp)
	fnstcw	-8(%rbp)
	movzwl	-8(%rbp),%edx
	and	$0b11110011,%dh			# RC (Rounding Control)
	or	$0b00000100,%dh			# →-∞
	mov	%dx,-4(%rbp)
	fxam
	fnstsw	%ax
	fabs
	test	$FPU_C1>>8,%ah
	fadds	.Lhalf(%rip)
	fldcw	-4(%rbp)
	fistpq	-16(%rbp)
	fldcw	-8(%rbp)
	mov	-16(%rbp),%rax
	je	1f
	neg	%rax
1:	leave
	ret
	.endfn	tinymath_lroundl,globl
	.alias	tinymath_lroundl,lroundl
	.alias	tinymath_lroundl,llroundl

	.rodata.cst4
.Lhalf:	.float	.5
