/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/macros.internal.h"

//	Loads previously saved processor state.
//
//	@param	rdi points to the jmp_buf
//	@param	esi is returned by setjmp() invocation (coerced nonzero)
//	@noreturn
//	@see	gclongjmp()
//	@see	siglongjmp()
	.ftrace1
longjmp:
	.ftrace2
_longjmp:
#ifdef __x86_64__
	mov	%esi,%eax
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
#elif defined(__aarch64__)
	ldp	x29,x30,[x0,#0]
	ldp	x2,x22,[x0,#16]
	ldp	x23,x24,[x0,#32]
	ldp	x25,x26,[x0,#48]
	ldp	x27,x28,[x0,#64]
	ldp	x19,x20,[x0,#80]
	ldr	x21,[x0,#104]
	mov	sp,x2
	ldp	d8 ,d9,[x0,#112]
	ldp	d10,d11,[x0,#128]
	ldp	d12,d13,[x0,#144]
	ldp	d14,d15,[x0,#160]
	cmp	w1,0
	csinc	w0,w1,wzr,ne
	br	x30
#else
#error "unsupported architecture"
#endif
	.endfn	longjmp,globl
	.endfn	_longjmp,globl
	.alias	longjmp,siglongjmp
