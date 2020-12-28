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
.privileged

/	Loads previously saved processor state.
/
/	@param	rdi points to the jmp_buf
/	@param	esi is returned by setjmp() invocation (coerced nonzero)
/	@noreturn
/	@assume	system five nexgen32e abi conformant
/	@note	code built w/ microsoft abi compiler can't call this
/	@see	gclongjmp() unwinds gc() destructors
longjmp:mov	%esi,%eax
	test	%eax,%eax
	jnz	1f
	inc	%eax
1:	mov	(%rdi),%rsp
	mov	8(%rdi),%rbx
	mov	16(%rdi),%rbp
	mov	24(%rdi),%r12
	mov	32(%rdi),%r13
	mov	40(%rdi),%r14
	mov	48(%rdi),%r15
	jmp	*56(%rdi)
	.endfn	longjmp,globl
	.source	__FILE__
