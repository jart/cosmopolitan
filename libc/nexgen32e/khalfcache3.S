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

#ifdef __x86_64__

	.initbss 202,_init_kHalfCache3
//	Half size of level 3 cache in bytes.
kHalfCache3:
	.quad	0
	.endobj	kHalfCache3,globl
	.previous

	.init.start 202,_init_kHalfCache3
	cmpl	$3,kCpuids(%rip)
	jbe	3f
	xor	%r8d,%r8d
	mov	$4,%r8d
1:	mov	%r8d,%eax
	mov	%r8d,%ecx
	push	%rbx
	cpuid
	mov	%ebx,%r9d
	pop	%rbx
	test	$31,%al
	je	3f
	cmp	$99,%al
	jne	2f
	mov	%r9d,%eax
	mov	%r9d,%edx
	inc	%ecx
	shr	$12,%r9d
	shr	$22,%eax
	and	$0x0fff,%edx
	and	$0x03ff,%r9d
	inc	%eax
	inc	%edx
	imul	%edx,%eax
	imul	%ecx,%eax
	lea	1(%r9),%ecx
	imul	%ecx,%eax
	jmp	4f
2:	inc	%r8d
	jmp	1b
3:	mov	$0x00400000,%eax
4:	shr	%eax
	stosq
	.init.end 202,_init_kHalfCache3

#else

	.rodata
	.balign	8
kHalfCache3:
	.quad	4 * 1024 * 1024
	.endobj	kHalfCache3,globl
	.previous

#endif /* __x86_64__ */
