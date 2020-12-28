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
#include "libc/calls/internal.h"
#include "libc/macros.h"
.source	__FILE__

/	System Five signal handler.
/
/	This is needed because (1) a signal is allowed to trigger at
/	just about any time, and leaf functions (e.g. memcpy) aren't
/	required to leave Cosmopolitan's image base register alone.
/
/	@param	%edi is the signal number
/	@param	%rsi will be passed for sigactions
/	@param	%rdx will be passed for sigactions
/	@return	true if handler was invoked
__sigenter:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	and	$NSIG-1,%edi
	mov	g_sighandrvas(,%rdi,4),%eax
	cmp	$kSigactionMinRva,%eax
	jl	2f
	lea	_base(%rax),%eax
	call	*%rax
	mov	$1,%eax
1:	leave
	ret
2:	xor	%eax,%eax
	jmp	1b
	.endfn	__sigenter,globl,hidden
