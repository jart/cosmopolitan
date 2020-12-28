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

/	Jump table for psrldq() with non-constexpr immediate parameter.
	.align	8
__psrldqs:
  	psrldq	$0,%xmm0
	ret
	nop
	nop
  	psrldq	$1,%xmm0
	ret
	nop
	nop
  	psrldq	$2,%xmm0
	ret
	nop
	nop
  	psrldq	$3,%xmm0
	ret
	nop
	nop
  	psrldq	$4,%xmm0
	ret
	nop
	nop
  	psrldq	$5,%xmm0
	ret
	nop
	nop
  	psrldq	$6,%xmm0
	ret
	nop
	nop
  	psrldq	$7,%xmm0
	ret
	nop
	nop
  	psrldq	$8,%xmm0
	ret
	nop
	nop
  	psrldq	$9,%xmm0
	ret
	nop
	nop
  	psrldq	$10,%xmm0
	ret
	nop
	nop
  	psrldq	$11,%xmm0
	ret
	nop
	nop
  	psrldq	$12,%xmm0
	ret
	nop
	nop
  	psrldq	$13,%xmm0
	ret
	nop
	nop
  	psrldq	$14,%xmm0
	ret
	nop
	nop
  	psrldq	$15,%xmm0
	ret
	nop
	nop
  	psrldq	$16,%xmm0
	ret
	.if	. - __psrldqs != 8 * 17 - 2
	.error	"bad assemblage"
	.endif
	.endfn	__psrldqs,globl
