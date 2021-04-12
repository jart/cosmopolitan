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
#include "libc/runtime/pc.internal.h"
#include "libc/macros.internal.h"

//	Returns 𝑒^𝑥-𝟷.
//
//	@param	𝑥 is an 80-bit long double passed on stack in 16-bytes
//	@return	result of exponentiation on FPU stack in %st
expm1l:	push	%rbp
	mov	%rsp,%rbp
	.profilable
	fldt	16(%rbp)
	fxam
	fstsw	%ax
	mov	%ah,%al
	and	$(FPU_C3|FPU_C2|FPU_C0)>>8,%ah
	cmp	$(FPU_C3)>>8,%ah		# !x
	je	0f
	cmp	$(FPU_C2|FPU_C0)>>8,%ah		# isinf(x)
	je	1f
	fldl2e
	fmulp	%st,%st(1)
	fld	%st
	frndint
	fsubr	%st,%st(1)
	fld1
	fxch	%st(2)
	f2xm1
	fscale
	fxch	%st(2)
	fscale
	fstp	%st(1)
	fld1
	fsubrp
	faddp	%st,%st(1)
0:	pop	%rbp
	ret
1:	test	$FPU_C1>>8,%al			# signbit(x)
	jz	0b
	fstp	%st
	fld1
3:	fchs
	jmp	0b
	.endfn	expm1l,globl
