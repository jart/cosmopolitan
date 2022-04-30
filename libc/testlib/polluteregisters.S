/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.internal.h"

__polluteregisters:
	.leafprologue
	xor	%eax,%eax
	xor	%ecx,%ecx
	xor	%edx,%edx
	xor	%r8d,%r8d
	xor	%r9d,%r9d
	xor	%r10d,%r10d
	xor	%r11d,%r11d
	testb	X86_HAVE(AVX)+kCpuids(%rip)
	jz	.Lsse
	vpxor	%xmm0,%xmm0,%xmm0
	vpxor	%xmm1,%xmm1,%xmm1
	vpxor	%xmm2,%xmm2,%xmm2
	vpxor	%xmm3,%xmm3,%xmm3
	vpxor	%xmm4,%xmm4,%xmm4
	vpxor	%xmm5,%xmm5,%xmm5
	vpxor	%xmm6,%xmm6,%xmm6
	vpxor	%xmm7,%xmm7,%xmm7
	.leafepilogue
.Lsse:	xorps	%xmm0,%xmm0
	xorps	%xmm1,%xmm1
	xorps	%xmm2,%xmm2
	xorps	%xmm3,%xmm3
	xorps	%xmm4,%xmm4
	xorps	%xmm5,%xmm5
	xorps	%xmm6,%xmm6
	xorps	%xmm7,%xmm7
	.leafepilogue
	.endfn	__polluteregisters,globl

	.end
//	Fill registers with junk data to create false dependencies.
//	Which shall create the problem that happens w/o vzeroupper.
//	Or the Core Architecture errata regarding BSR/BSF w/ 64bit.
__polluteregisters:
	.leafprologue
	mov	$-1,%rax
	mov	%rax,%rcx
	mov	%rax,%rdx
	mov	%rax,%r8
	mov	%rax,%r9
	mov	%rax,%r10
	mov	%rax,%r11
	movq	%rax,%xmm0
	testb	X86_HAVE(AVX)+kCpuids(%rip)
	jz	.Lsse
	vmovq	%r8,%xmm0
	vmovq	%r9,%xmm1
	vmovq	%r10,%xmm2
	vmovq	%r11,%xmm3
	vmovq	%r12,%xmm4
	vmovq	%r13,%xmm5
	vmovq	%r14,%xmm6
	vmovq	%r15,%xmm7
	vinsertf128 $0x1,%xmm0,%ymm0,%ymm0
	vinsertf128 $0x1,%xmm1,%ymm1,%ymm1
	vinsertf128 $0x1,%xmm2,%ymm2,%ymm2
	vinsertf128 $0x1,%xmm3,%ymm3,%ymm3
	vinsertf128 $0x1,%xmm4,%ymm4,%ymm4
	vinsertf128 $0x1,%xmm5,%ymm5,%ymm5
	vinsertf128 $0x1,%xmm6,%ymm6,%ymm6
	vinsertf128 $0x1,%xmm7,%ymm7,%ymm7
	.leafepilogue
.Lsse:	punpcklqdq %xmm0,%xmm0
	punpcklqdq %xmm0,%xmm1
	punpcklqdq %xmm0,%xmm2
	punpcklqdq %xmm0,%xmm3
	punpcklqdq %xmm0,%xmm4
	punpcklqdq %xmm0,%xmm5
	punpcklqdq %xmm0,%xmm6
	punpcklqdq %xmm0,%xmm7
	.leafepilogue
	.endfn	__polluteregisters,globl
