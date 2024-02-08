/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

//	Invokes system call w/ arity of three.
//
//	This function takes four params. The first three are for
//	args passed along to the system call. The 4th is for the
//	the magic number, indicating which system call is called
//
//	The return value follows the Linux Kernel (System V) ABI
//	where -errno is returned, rather than doing -1 w/ errno.
//
//	This helper should not be used to do cancelation points.
__syscall3:
#ifdef __aarch64__
	mov	x8,x3				// syscall number (linux)
	mov	x16,x3				// syscall number (xnu)
	mov	x9,0				// clear carry flag
	adds	x9,x9,0				// clear carry flag
	svc	0
	bcs	1f
	ret
1:	neg	x0,x0
	ret
#elif defined(__x86_64__)
	mov	%ecx,%eax			// arg4 -> syscall number
	clc					// linux saves carry flag
	syscall					// bsds set carry on errs
	jnc	1f
	neg	%rax				// normalizes to system v
1:	ret
#else
#error "unsupported architecture"
#endif
	.endfn	__syscall3,globl
