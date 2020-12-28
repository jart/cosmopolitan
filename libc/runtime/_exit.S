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
#include "libc/runtime/internal.h"
#include "libc/macros.h"
.privileged
.source	__FILE__

/	Terminates process, ignoring destructors and atexit() handlers.
/
/	@param	edi is exit code ∈ [0,256)
/	@note	_exit() is same thing
/	@asyncsignalsafe
_Exit:	push	%rbp
	mov	%rsp,%rbp
#if SupportsWindows()
	testb	IsWindows()
	jz	1f
	sub	$32,%rsp
	movzbl	%dil,%ecx		# %ERRORLEVEL% is limitless
4:	call	*__imp_ExitProcess(%rip)
	jmp	4b
0:	int3				# @see setjmp() in WinMain()
#endif
1:	mov	__NR_exit(%rip),%eax
	syscall
	cli
	lidt	3f
2:	hlt
	jmp	2b
3:	.quad	0
	.endfn	_Exit,globl,protected
	.hidden	__NR_exit
