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
.text.windows

//	Used by clone() on Windows to launch thread.
//
//	Windows owns the stack memory when we initially enter threads.
//	This function switches us over, so that we can start using the
//	runtime facilities.
//
//	@param	%rdi is arg
//	@param	%rsi is tid
//	@param	%rdx is func
//	@param	%rcx is stack
//	@return	%rax is exit code
//	@see	clone()
WinThreadLaunch:
	push	%rbx
	push	%r15
	mov	%rbp,%r15
	mov	%rsp,%rbx
	mov	%rcx,%rsp
	xor	%rbp,%rbp
	call	*%rdx
	mov	%r15,%rbp
	mov	%rbx,%rsp
	pop	%r15
	pop	%rbx
	ret
	.endfn	WinThreadLaunch,globl,hidden
