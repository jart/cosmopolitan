/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/sysv/consts/nr.h"
#include "libc/macros.internal.h"
.privileged

_futex:
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	mov	%rcx,%r10
	mov	__NR_futex,%eax
	clc
	syscall
	jnc	1f
	neg	%eax
1:	pop	%rbp
#elif defined(__aarch64__)
	ldr	x7,=__hostos
	ldr	w7,[x7]
	tst	x7,1			// IsLinux()
	mov	x7,98			// futex (Linux)
	mov	x8,454			// _umtx_op (FreeBSD)
	csel	x8,x7,x8,ne		// choose syscall magnum
	mov	x7,0			// clear carry (for Linux)
	adds	x7,x7,0			// clear carry
	svc	#0			// call kernel
	bcc	1f			// jump if not carry
	neg	x0,x0			// linux style errno
#else
#error "unsupported architecture"
#endif /* __x86_64__ */
1:	ret
	.endfn	_futex,globl,hidden
