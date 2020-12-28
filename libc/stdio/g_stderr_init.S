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
#include "libc/sysv/consts/o.h"
#include "libc/dce.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/macros.h"
.source	__FILE__

	.init.start 400,_init_g_stderr
	ezlea	g_stderr,ax
	push	$_IOLBF
	pop	(%rax)				#→ f.fd
	push	STDERR_FILENO
	pop	12(%rax)
	mov	O_WRONLY,%edx
	mov	%edx,4(%rax)			#→ f.iomode
	ezlea	g_stderr_buf,cx
	mov	%rcx,24(%rax)			#→ f.buf
	movl	$BUFSIZ,32(%rax)		#→ f.size
	ezlea	__fwritebuf,cx
	mov	%rcx,%rdx
	mov	%rdx,48(%rax)			#→ f.writer
	mov	%rax,stderr(%rip)
	.init.end 400,_init_g_stderr,globl,hidden
