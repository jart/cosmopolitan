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
#include "libc/nexgen32e/x86feature.h"
#include "libc/bits/smmintrin.internal.h"
#include "libc/macros.h"
.source	__FILE__

/	Rounds to nearest integer.
/
/	@param	is double passed in %xmm0
/	@return	double in %xmm0
/	@note	rounding behavior can be changed in mxcsr
tinymath_rint:
#if !X86_NEED(SSE4_2)
	testb	X86_HAVE(SSE4_2)+kCpuids(%rip)
	jz	tinymath_rint$k8
	.text.antiquity
tinymath_rint$k8:
0:	movq	%xmm0,%rax
	movq	%xmm0,%rdx
	shr	$52,%rdx
	and	$2047,%edx
	cmp	$1074,%edx
	jg	2f
	movsd	mmm(%rip),%xmm1
	shr	$63,%rax
	jne	3f
	addsd	%xmm1,%xmm0
	subsd	%xmm1,%xmm0
1:	pxor	%xmm2,%xmm2
	ucomisd	%xmm2,%xmm0
	jp	2f
	jne	2f
	movsd	sgn(%rip),%xmm0
	test	%rax,%rax
	je	4f
2:	ret
3:	subsd	%xmm1,%xmm0
	addsd	%xmm1,%xmm0
	jmp	1b
4:	pxor	%xmm0,%xmm0
	ret
	.endfn	tinymath_rint$k8,globl,hidden
	.previous
	.rodata.cst8
sgn:	.quad	0x8000000000000000
mmm:	.quad	0x4330000000000000
	.previous
#endif
	roundsd $_MM_FROUND_RINT,%xmm0,%xmm0
	ret
	.endfn	tinymath_rint,globl
	.alias	tinymath_rint,rint
