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

tinymath_ceilf:
	.leafprologue
	.profilable
	movss	.L3(%rip),%xmm1
	andps	%xmm0,%xmm1
	movss	.L2(%rip),%xmm2
	comiss	%xmm1,%xmm2
	jbe	1f
	cvttss2si %xmm0,%eax
	pxor	%xmm1,%xmm1
	movss	.L1(%rip),%xmm2
	cvtsi2ss %eax,%xmm1
	cmpnless %xmm1,%xmm0
	andps	%xmm2,%xmm0
	addss	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_ceilf,globl
	.alias	tinymath_ceilf,ceilf

	.rodata.cst4
.L1:	.float	1.0
.L2:	.long	1258291200

	.rodata.cst16
.L3:	.long	2147483647
	.long	0
	.long	0
	.long	0

/	TODO(jart):
/	vroundss	$10,%xmm0,%xmm0,%xmm0
