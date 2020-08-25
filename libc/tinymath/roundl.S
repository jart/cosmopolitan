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

/	Rounds to nearest integer, away from zero.
/
/	@param	𝑥 is an 80-bit long double passed on stack in 16-bytes
/	@return	result of exponentiation on FPU stack in %st
tinymath_roundl:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%rax
	fldt	16(%rbp)
	fnstcw	-8(%rbp)
	movzwl	-8(%rbp),%edx
	and	$0b11110011,%dh			# RC (Rounding Control)
	or	$0b00000100,%dh			# →-∞ a.k.a. floor()
	mov	%dx,-4(%rbp)
	fxam					# C1 is set to sign bit
	fnstsw
	fabs
	test	$FPU_C1>>8,%ah
	fadds	.Lhalf(%rip)
	fldcw	-4(%rbp)
	frndint
	fldcw	-8(%rbp)
	je	1f
	fchs
1:	leave
	ret
	.endfn	tinymath_roundl,globl
	.alias	tinymath_roundl,roundl
	.source	__FILE__

	.rodata.cst4
.Lhalf:	.float	.5
