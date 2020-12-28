/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
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
	fnstcw	-6(%rbp)
	fnstcw	-8(%rbp)
	orb	$0b00001100,-7(%rbp)		# RC = →0
	fxam					# C1 is set to sign bit
	fnstsw
	fabs
	test	$FPU_C1>>8,%ah
	fadds	.Lhalf(%rip)
	fldcw	-8(%rbp)
	frndint
	fldcw	-6(%rbp)
	je	1f
	fchs
1:	leave
	ret
	.endfn	tinymath_roundl,globl
	.alias	tinymath_roundl,roundl
	.source	__FILE__

	.rodata.cst4
.Lhalf:	.float	.5
