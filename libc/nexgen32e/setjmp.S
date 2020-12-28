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

/	Saves caller CPU state to cacheline.
/
/	@param	rdi points to jmp_buf
/	@return	rax 0 when set and !0 when longjmp'd
/	@returnstwice
/	@assume	system five nexgen32e abi conformant
/	@note	code built w/ microsoft abi compiler can't call this
/	@see	longjmp(), gclongjmp()
setjmp:	lea	8(%rsp),%rax
	mov	%rax,(%rdi)
	mov	%rbx,8(%rdi)
	mov	%rbp,16(%rdi)
	mov	%r12,24(%rdi)
	mov	%r13,32(%rdi)
	mov	%r14,40(%rdi)
	mov	%r15,48(%rdi)
	mov	(%rsp),%rax
	mov	%rax,56(%rdi)
	xor	%eax,%eax
	ret
	.endfn	setjmp,globl
	.source	__FILE__
