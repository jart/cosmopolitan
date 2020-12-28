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

tinymath_rintf:
	.leafprologue
	.profilable
	movaps	%xmm0,%xmm1
	movss	.LC8(%rip),%xmm2
	andps	%xmm2,%xmm1
	movss	.LC7(%rip),%xmm3
	comiss	%xmm1,%xmm3
	jbe	1f
	addss	%xmm3,%xmm1
	andnps	%xmm0,%xmm2
	movaps	%xmm2,%xmm0
	subss	%xmm3,%xmm1
	orps	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_rintf,globl
	.alias	tinymath_rintf,rintf

	.rodata.cst4
.LC7:	.long	1258291200

	.rodata.cst16
.LC8:	.long	2147483647
	.long	0
	.long	0
	.long	0

/	TODO(jart):
/	vroundss	$4,%xmm0,%xmm0,%xmm0
