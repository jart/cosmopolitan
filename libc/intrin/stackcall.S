/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

//	Calls function on different stack.
//
//	@param	%rdi is arg1
//	@param	%rsi is arg2
//	@param	%rdx is arg3
//	@param	%rcx is arg4
//	@param	%r8 is func
//	@param	%r9 is stack
//	@return	%rax is res
__stack_call:
#ifdef __x86_64__
	push	%rbx
	push	%r15
	mov	%rbp,%r15
	mov	%rsp,%rbx
	mov	%r9,%rsp
	xor	%rbp,%rbp
	call	*%r8
	mov	%r15,%rbp
	mov	%rbx,%rsp
	pop	%r15
	pop	%rbx
	ret
#elif defined(__aarch64__)
	stp	x29,x30,[sp,#-32]!
	str	x27,[sp,16]
	mov	x27,sp
	and	sp,x5,#-16
	mov	x29,0
	blr	x4
	mov	sp,x27
	ldr	x27,[sp,16]
	ldp	x29,x30,[sp],#32
	ret
#else
#error "unsupported architecture"
#endif
	.endfn	__stack_call,globl
