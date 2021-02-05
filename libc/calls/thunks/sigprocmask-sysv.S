/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/macros.h"

/	Sets System Five process signal mask w/ standard ABI.
sys_sigprocmask:
	push	%rbp
	mov	%rsp,%rbp
#if SupportsOpenbsd()
	testb	IsOpenbsd()
	jz	4f
	test	%rsi,%rsi
	jnz	1f
	mov	$1,%edi				# SIG_BLOCK on BSDs
	jmp	2f
1:	mov	(%rsi),%esi			# openbsd:byvalue
2:	call	__sys_sigprocmask
	cmp	$-1,%eax
	je	5f
	test	%rdx,%rdx			# original param not a result
	jz	3f
	mov	%eax,(%rdx)			# openbsd:byvalue
3:	xor	%eax,%eax
	jmp	5f
#endif
4:	call	__sys_sigprocmask
5:	pop	%rbp
	ret
	.endfn	sys_sigprocmask,globl
