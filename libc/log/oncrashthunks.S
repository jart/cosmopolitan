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

/	These function names make it a bit more obvious which signal
/	caused the crash, particularly in the GDB GUI. They're coded
/	into an array to pinch pennies on code size registering them.

__oncrash_thunks:

	.org	11*0
__oncrash_sigquit:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigquit,globl

	.org	11*1
__oncrash_sigfpe:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigfpe,globl

	.org	11*2
__oncrash_sigill:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigill,globl

	.org	11*3
__oncrash_sigsegv:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigsegv,globl

	.org	11*4
__oncrash_sigtrap:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigtrap,globl

	.org	11*5
__oncrash_sigabrt:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigabrt,globl

	.org	11*6
__oncrash_sigbus:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigbus,globl

	.endobj	__oncrash_thunks,globl
