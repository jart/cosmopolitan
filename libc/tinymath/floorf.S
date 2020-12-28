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
.source	__FILE__

tinymath_floorf:
	.leafprologue
	.profilable
	movss	.LC8(%rip),%xmm1
	andps	%xmm0,%xmm1
	movss	.LC7(%rip),%xmm2
	comiss	%xmm1,%xmm2
	jbe	1f
	cvttss2si %xmm0,%eax
	pxor	%xmm1,%xmm1
	movss	.LC3(%rip),%xmm2
	cvtsi2ss %eax,%xmm1
	movaps	%xmm1,%xmm3
	cmpnless %xmm0,%xmm3
	movaps	%xmm3,%xmm0
	andps	%xmm2,%xmm0
	subss	%xmm0,%xmm1
	movaps	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_floorf,globl
	.alias	tinymath_floorf,floorf

	.rodata.cst4
.LC3:	.float	1.0
.LC7:	.long	0x4b000000

	.rodata.cst16
.LC8:	.long	2147483647,0,0,0
