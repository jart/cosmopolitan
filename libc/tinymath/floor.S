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

/	vroundsd $_MM_FROUND_TO_NEG_INF|_MM_FROUND_NO_EXC,%xmm0,%xmm0,%xmm0

tinymath_floor:
	.leafprologue
	.profilable
	movsd	4f(%rip),%xmm1
	movsd	3f(%rip),%xmm2
	andpd	%xmm0,%xmm1
	comisd	%xmm1,%xmm2
	jbe	1f
	cvttsd2siq %xmm0,%rax
	pxor	%xmm1,%xmm1
	movsd	2f(%rip),%xmm2
	cvtsi2sdq %rax,%xmm1
	movapd	%xmm1,%xmm3
	cmpnlesd %xmm0,%xmm3
	movapd	%xmm3,%xmm0
	andpd	%xmm2,%xmm0
	subsd	%xmm0,%xmm1
	movapd	%xmm1,%xmm0
1:	.leafepilogue
	.endfn	tinymath_floor,globl
	.alias	tinymath_floor,floor

	.rodata.cst8
2:	.double	1
3:	.double	0x0010000000000000
4:	.double	nan
