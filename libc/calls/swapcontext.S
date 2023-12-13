/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"

//	Saves machine to 𝑥 and activates 𝑦, i.e.
//
//	    getcontext(x);
//	    setcontext(y);
//
//	Except using this API is safer and goes 2x faster:
//
//	    swapcontext(x, y);
//
//	@return	0 on success, or -1 w/ errno
//	@returnstwice
	.ftrace1
swapcontext:
	.ftrace2
#include "libc/calls/getcontext.inc"
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	push	%rsi
	push	%rsi
	call	__swapcontextsig
	pop	%rdi
	pop	%rdi
	pop	%rbp
	test	%eax,%eax
	jnz	1f
#elif defined(__aarch64__)
	stp	x29,x30,[sp,#-32]!
	mov	x29,sp
	str	x1,[sp,16]
	bl	__swapcontextsig
	ldr	x1,[sp,16]
	ldp	x29,x30,[sp],#32
	cbnz	w0,1f
	mov	x0,x1
#endif
	jmp	__tailcontext
1:	ret
	.endfn	swapcontext,globl
