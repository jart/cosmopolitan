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

//	Returns index of minimum positive int16 in array.
//
//	@param	rdi points to nonempty array
//	@param	esi is 16-byte aligned 8+ / 8 multiple array item count
//	@note	needs sse4 (nehalem+)
	.ftrace1
windex_sse4:
	.ftrace2
	push	%rbp
	mov	%rsp,%rbp
	mov	$8,%eax
	sub	$32,%rsp
	movdqa	(%rdi),%xmm2
	movdqa	.Lidx(%rip),%xmm1
	movdqa	.Linc(%rip),%xmm6
	movdqa	%xmm1,%xmm3
0:	cmp	%eax,%esi
	je	1f
	add	$8,%eax
	movdqa	-16(%rdi,%rax,2),%xmm4
	movdqa	%xmm2,%xmm7
	movdqa	%xmm3,%xmm5
	paddw	%xmm6,%xmm5
	movdqa	%xmm5,%xmm3
	pcmpgtw	%xmm4,%xmm7
	pminsw	%xmm4,%xmm2
	movdqa	%xmm7,%xmm0
	pblendvb %xmm0,%xmm5,%xmm1
	jmp	0b
1:	phminposuw %xmm2,%xmm0
	movd	%xmm0,%eax
	movdqa	%xmm1,-32(%rbp)
	shr	$16,%eax
	movzwl	-32(%rbp,%rax,2),%eax
	leave
	ret
	.endfn	windex_sse4,globl

	.rodata.cst16
.Lidx:	.short	0,1,2,3,4,5,6,7
.Linc:	.value	8,8,8,8,8,8,8,8
