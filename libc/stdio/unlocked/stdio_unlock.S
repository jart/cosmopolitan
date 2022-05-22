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

//	Wrapper for applying locking to stdio functions.
//
//	This function is intended to be called by thunks.
//
//	@param	rax is stdio function pointer
//	@param	rdi is passed along as an arg
//	@param	rsi is passed along as an arg
//	@param	rdx is passed along as an arg
//	@param	rcx is passed along as an arg
//	@param	r11 has the FILE* obj pointer
//	@return	rax is passed along as result
//	@return	rdx is passed along as result
//	@threadsafe
stdio_unlock:
	push	%rbp
	mov	%rsp,%rbp
	.profilable

//	acquires mutex
	push	%rax
	push	%rdi
	push	%rsi
	push	%rdx
	push	%rcx
	push	%r11
	mov	%r11,%rdi
	call	flockfile
	pop	%r11
	pop	%rcx
	pop	%rdx
	pop	%rsi
	pop	%rdi
	pop	%rax

//	calls delegate
	push	%r11
	push	%rsi			# align stack
	call	*%rax
	pop	%rsi
	pop	%r11

//	releases mutex
	push	%rax
	push	%rdx
	mov	%r11,%rdi
	call	funlockfile
	pop	%rdx
	pop	%rax

	pop	%rbp
	ret
	.endfn	stdio_unlock,globl
