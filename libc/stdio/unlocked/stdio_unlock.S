/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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

#define LOCK 0x2c /* see struct file in stdio.h */

//	Wrapper for applying locking to stdio functions.
//
//	This function is intended to be called by thunks.
//
//	@param	rax has the delegate function pointer
//	@param	rdi is passed along as an arg
//	@param	rsi is passed along as an arg
//	@param	rdx is passed along as an arg
//	@param	rcx is passed along as an arg
//	@param	r8  is passed along as an arg
//	@param	r9  is passed along as an arg
//	@param	r10 is passed along as an arg
//	@param	r11 has the FILE* obj pointer
//	@return	rax is passed along as result
//	@return	rdx is passed along as result
stdio_unlock:
	push	%rbp
	mov	%rsp,%rbp

//	acquires mutex
	push	%rcx
	push	%rdx
	mov	$1,%cl
0:	mov	LOCK(%r11),%dl			# optimistic
	test	%dl,%dl
	je	2f
1:	pause					# hyperyield
	jmp	0b
2:	mov	%ecx,%edx
	xchg	LOCK(%r11),%dl			# locks bus!
	test	%dl,%dl
	jne	1b
	pop	%rdx
	pop	%rcx

//	calls delegate
	push	%rsi
	push	%r11
	call	*%rax
	pop	%r11
	pop	%rsi

//	releases mutex
	movb	$0,LOCK(%r11)

	pop	%rbp
	ret
	.endfn	stdio_unlock,globl
