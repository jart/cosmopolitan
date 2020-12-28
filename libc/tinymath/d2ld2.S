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
#include "libc/macros.h"
.source	__FILE__

/	Thunks double(*fn)(double,double) -> long double fn.
/
/	@param	%xmm0[0] contains double param
/	@return	%xmm0[0] contains double result
/	@note	100% negligible overhead
_d2ld2:	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	movsd	%xmm0,-32(%rbp)
	fldl	-32(%rbp)
	fstpt	-32(%rbp)
	movsd	%xmm1,-16(%rbp)
	fldl	-16(%rbp)
	fstpt	-16(%rbp)
	call	*%rax
	fstpl	-16(%rbp)
	movsd	-16(%rbp),%xmm0
	leave
	ret
	.endfn	_d2ld2,globl,hidden
