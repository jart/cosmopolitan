/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/nexgen32e/x86feature.h"
#include "libc/bits/smmintrin.h"
#include "libc/macros.h"
.source	__FILE__

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
