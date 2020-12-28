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
#include "libc/macros.h"

/	Jump table for pslldq() with non-constexpr immediate parameter.
	.align	8
__pslldqs:
  	pslldq	$0,%xmm0
	ret
	nop
	nop
  	pslldq	$1,%xmm0
	ret
	nop
	nop
  	pslldq	$2,%xmm0
	ret
	nop
	nop
  	pslldq	$3,%xmm0
	ret
	nop
	nop
  	pslldq	$4,%xmm0
	ret
	nop
	nop
  	pslldq	$5,%xmm0
	ret
	nop
	nop
  	pslldq	$6,%xmm0
	ret
	nop
	nop
  	pslldq	$7,%xmm0
	ret
	nop
	nop
  	pslldq	$8,%xmm0
	ret
	nop
	nop
  	pslldq	$9,%xmm0
	ret
	nop
	nop
  	pslldq	$10,%xmm0
	ret
	nop
	nop
  	pslldq	$11,%xmm0
	ret
	nop
	nop
  	pslldq	$12,%xmm0
	ret
	nop
	nop
  	pslldq	$13,%xmm0
	ret
	nop
	nop
  	pslldq	$14,%xmm0
	ret
	nop
	nop
  	pslldq	$15,%xmm0
	ret
	nop
	nop
  	pslldq	$16,%xmm0
	ret
	.if	. - __pslldqs != 8 * 17 - 2
	.error	"bad assemblage"
	.endif
	.endfn	__pslldqs,globl
