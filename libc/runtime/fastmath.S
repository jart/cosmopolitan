/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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
#include "libc/notice.inc"
.text.startup
.source	__FILE__

/	Intel Manual V.1 §10.2.3 - MXCSR Control and Status Register
#define MXCSR_DE (1 << 1)	/* Denormal Flag */
#define MXCSR_DAZ (1 << 6)	/* Denormals Are Zeros */
#define MXCSR_DM (1 << 8)	/* Denormal Operation Mask */
#define MXCSR_FTZ (1 << 15)	/* Flush to Zero */

/	Initializes fast math.
/
/	 “Seymour Cray didn't care that 81.0/3.0 did not give exactly
/	  27.0 on the CDC 6000 class machines, and he was universally
/	  respected for making the fastest machines around.
/	                                        ──Linus Torvalds
__fast_math:
	push	%rbp
	mov	%rsp,%rbp
	push	%rax
	stmxcsr	(%rsp)
	orl	$MXCSR_FTZ+MXCSR_DAZ,(%rsp)
	ldmxcsr	(%rsp)
	leave
	ret
	.endfn	__fast_math,globl
