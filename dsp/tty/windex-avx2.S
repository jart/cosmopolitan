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
#include "libc/macros.h"
.yoink	__FILE__

/	Returns index of minimum uint16 in array.
/
/	@param	rdi points to nonempty array
/	@param	rsi is item count divisible by 16
/	@note	needs avx2 (haswell+)
windex$avx2:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
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
	.endfn	windex$avx2,globl

	.rodata.cst32
.Lidx:	.short	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
.Linc:	.value	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
