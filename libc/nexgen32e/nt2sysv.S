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
.text.windows

//	Translates function call from code built w/ MS-style compiler.
//
//	This wraps WinMain() and callback functions passed to Win32 API.
//	Please note an intermediary jump slot is needed to set %rax.
//
//	@param	%rax is function address
//	@param	%rcx,%rdx,%r8,%r9
//	@return	%rax,%xmm0
//	@note	slower than __sysv2nt
//	@see	NT2SYSV() macro
__nt2sysv:
	push	%rbp
	mov	%rsp,%rbp
	sub	$256,%rsp
	push	%rbx
	push	%rdi
	push	%rsi
	pushf					// TODO(jart): Do we need it?
	call	__xmm_save
	mov	%rcx,%rdi
	mov	%rdx,%rsi
	mov	%r8,%rdx
	mov	%r9,%rcx
	call	*%rax
	call	__xmm_load
	popf
	pop	%rsi
	pop	%rdi
	pop	%rbx
	leave
	ret
	.endfn	__nt2sysv,globl,hidden
