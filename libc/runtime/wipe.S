/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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

//	Zeroes as many registers as possible.
//
//	Each caller should declare an appropriate prototype.
//
//	@param	is return value
//	@return	is copied from parameter
__wipe:
#ifdef __x86_64__
	mov	%rdi,%rax
	xor	%edi,%edi
	xor	%esi,%esi
	xor	%edx,%edx
	xor	%ecx,%ecx
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
	ret
.Lsse:	xorps	%xmm0,%xmm0
	xorps	%xmm1,%xmm1
	xorps	%xmm2,%xmm2
	xorps	%xmm3,%xmm3
	xorps	%xmm4,%xmm4
	xorps	%xmm5,%xmm5
	xorps	%xmm6,%xmm6
	xorps	%xmm7,%xmm7
	ret
#elif defined(__aarch64__)
	mov	x1,#0
	mov	x2,#0
	mov	x3,#0
	mov	x4,#0
	mov	x5,#0
	mov	x6,#0
	mov	x7,#0
	mov	x9,#0
	mov	x10,#0
	mov	x11,#0
	mov	x12,#0
	mov	x13,#0
	mov	x14,#0
	mov	x15,#0
	movi	v0.16b,#0
	movi	v1.16b,#0
	movi	v2.16b,#0
	movi	v3.16b,#0
	movi	v4.16b,#0
	movi	v5.16b,#0
	movi	v6.16b,#0
	movi	v7.16b,#0
	ret
#endif
	.endfn	__wipe,globl,hidden
