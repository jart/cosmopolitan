/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/macros.internal.h"

//	Jump table for palignr() with non-constexpr immediate parameter.
//
//	@note	needs ssse3 cf. prescott c. 2004 cf. bulldozer c. 2011
//	@see	palignr()
	.balign	8
__palignrs:
  	palignr	$0,%xmm1,%xmm0
	ret
	nop
  	palignr	$1,%xmm1,%xmm0
	ret
	nop
  	palignr	$2,%xmm1,%xmm0
	ret
	nop
  	palignr	$3,%xmm1,%xmm0
	ret
	nop
  	palignr	$4,%xmm1,%xmm0
	ret
	nop
  	palignr	$5,%xmm1,%xmm0
	ret
	nop
  	palignr	$6,%xmm1,%xmm0
	ret
	nop
  	palignr	$7,%xmm1,%xmm0
	ret
	nop
  	palignr	$8,%xmm1,%xmm0
	ret
	nop
  	palignr	$9,%xmm1,%xmm0
	ret
	nop
  	palignr	$10,%xmm1,%xmm0
	ret
	nop
  	palignr	$11,%xmm1,%xmm0
	ret
	nop
  	palignr	$12,%xmm1,%xmm0
	ret
	nop
  	palignr	$13,%xmm1,%xmm0
	ret
	nop
  	palignr	$14,%xmm1,%xmm0
	ret
	nop
  	palignr	$15,%xmm1,%xmm0
	ret
	nop
  	palignr	$16,%xmm1,%xmm0
	ret
	nop
  	palignr	$17,%xmm1,%xmm0
	ret
	nop
  	palignr	$18,%xmm1,%xmm0
	ret
	nop
  	palignr	$19,%xmm1,%xmm0
	ret
	nop
  	palignr	$20,%xmm1,%xmm0
	ret
	nop
  	palignr	$21,%xmm1,%xmm0
	ret
	nop
  	palignr	$22,%xmm1,%xmm0
	ret
	nop
  	palignr	$23,%xmm1,%xmm0
	ret
	nop
  	palignr	$24,%xmm1,%xmm0
	ret
	nop
  	palignr	$25,%xmm1,%xmm0
	ret
	nop
  	palignr	$26,%xmm1,%xmm0
	ret
	nop
  	palignr	$27,%xmm1,%xmm0
	ret
	nop
  	palignr	$28,%xmm1,%xmm0
	ret
	nop
  	palignr	$29,%xmm1,%xmm0
	ret
	nop
  	palignr	$30,%xmm1,%xmm0
	ret
	nop
  	palignr	$31,%xmm1,%xmm0
	ret
	.if	. - __palignrs != 8 * 32 - 1
	.error	"bad assemblage"
	.endif
	.endfn	__palignrs,globl
