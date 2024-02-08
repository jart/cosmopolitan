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
#include "libc/macros.internal.h"

//	Checks that stack is 16-byte aligned.
//
//	This function crashes if called with a misaligned stack.
CheckStackIsAligned:
#ifdef __x86_64__

	push	%rbp
	mov	%rsp,%rbp
	test	$15,%rsp
	jz	1f
	int3				// misaligned stack trap
1:	pop	%rbp
	ret

#elif defined(__aarch64__)

	stp	x29,x30,[sp,#-16]!
	mov	x29,sp
	and	x0,x29,#15
	cbz	x0,1f
	brk	#666			// misaligned stack trap
1:	ldp	x29,x30,[sp],#16
	ret

#else
#error "unsupported architecture"
#endif
	.endfn	CheckStackIsAligned,globl
