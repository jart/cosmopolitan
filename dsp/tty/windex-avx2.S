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

//	Returns index of minimum uint16 in array.
//
//	@param	rdi points to nonempty array
//	@param	rsi is item count divisible by 16
//	@note	needs avx2 (haswell+)
	.ftrace1
windex_avx2:
	.ftrace2
	push	%rbp
	mov	%rsp,%rbp
	and	$-32,%rsp
	sub	$32,%rsp
	vmovdqa	(%rdi),%ymm1
	vmovdqa	.Lidx(%rip),%ymm3
	vmovdqa	.Linc(%rip),%ymm5
	cmp	$16,%esi
	jbe	2f
	vmovdqa	%ymm3,%ymm0
	mov	$16,%eax
3:	vpaddw	%ymm0,%ymm5,%ymm0
	mov	%eax,%edx
	vmovdqa	(%rdi,%rdx,2),%ymm2
	vpcmpgtw %ymm2,%ymm1,%ymm4
	vpblendvb %ymm4,%ymm0,%ymm3,%ymm3
	vpminsw	%ymm1,%ymm2,%ymm1
	add	$16,%eax
	cmp	%eax,%esi
	ja	3b
2:	vphminposuw %xmm1,%xmm0
	vextracti128 $0x1,%ymm1,%xmm1
	vphminposuw %xmm1,%xmm1
	vmovdqa	%ymm3,(%rsp)
	vmovq	%xmm0,%rdx
	vmovq	%xmm1,%rax
	cmp	%dx,%ax
	jbe	4f
	sar	$16,%rdx
	movzwl	%dx,%edx
	movzwl	(%rsp,%rdx,2),%eax
	jmp	5f
4:	sar	$16,%rax
	movzwl	%ax,%eax
	movzwl	16(%rsp,%rax,2),%eax
5:	vzeroupper
	leave
	ret
	.endfn	windex_avx2,globl

	.rodata.cst32
.Lidx:	.short	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
.Linc:	.value	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
