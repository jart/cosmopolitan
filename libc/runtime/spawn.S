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
#include "libc/dce.h"
#include "libc/sysv/consts/prot.h"
#include "libc/macros.h"

/	Self-bootstraps process upon existence before calling main.
/
/	@param	r12 is argc
/	@param	r13 is argv
/	@param	r14 is environ
/	@param	r15 is auxv
_spawn:	push	%rbp
	mov	%rsp,%rbp

/	Tune FPU settings if -ffast-math is somehow used systemically.
#ifdef __FAST_MATH__
	call	__fast_math
#endif

/	Call decentralized initialization assembly.
	call	_init
#if IsModeDbg()
	call	_init				# _init() is idempotent
#endif

/	Call global initialization functions.
	call	_construct

/	Restricts .initbss memory so it's read-only after initialization.
/	TODO: Delete this unless there's measurable performance advantage.
#if !IsTrustworthy()
	mov	$PROT_READ,%edi
	call	_piro
#endif

	pop	%rbp
	ret
	.endfn	_spawn,globl
