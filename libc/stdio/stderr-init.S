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
#include "libc/thread/thread.h"
#include "libc/macros.internal.h"

	.init.start 400,_init_stderr
	ezlea	__stderr,ax
	push	$_IOLBF
	pop	(%rax)					#→ f.fd
	push	STDERR_FILENO
	pop	12(%rax)
	mov	O_WRONLY,%edx
	mov	%edx,4(%rax)				#→ f.iomode
	lea	0x50(%rax),%rcx				#← f.mem
	mov	%rcx,0x18(%rax)				#→ f.buf
	movl	$BUFSIZ,0x20(%rax)			#→ f.size
	movb	$PTHREAD_MUTEX_RECURSIVE,0x38+4(%rax)	#→ f.lock._type
	mov	%rax,stderr(%rip)
	.init.end 400,_init_stderr,globl,hidden
