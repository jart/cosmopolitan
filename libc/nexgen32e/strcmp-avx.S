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

/	Compares NUL-terminated strings w/ AVX (2011+)
/
/	@param	rdi first string
/	@param	rsi second string
/	@return	0 if equal, etc.
/	@asyncsignalsafe
strcmp$avx:
	or	$-1,%rdx
/	fallthrough
	.endfn	strcmp$avx,globl,hidden

/	Compares NUL-terminated strings, with byte limit.
/
/	@param	rdi first string
/	@param	rsi second string
/	@param	rdx maximum number of bytes to compare
/	@return	0 if equal, etc.
/	@asyncsignalsafe
strncmp$avx:
	.leafprologue
	.profilable
	cmp	%rsi,%rdi
	je	1f
	mov	$-16,%rcx
	vxorps	%xmm0,%xmm0,%xmm0
	vpcmpeqd %xmm1,%xmm1,%xmm1
3:	add	$16,%rcx
4:	lea	16(%rcx),%rax
	cmp	%rdx,%rax
	ja	9f
	lea	(%rdi,%rcx),%eax
	and	$0xfff,%eax
	cmp	$0xff0,%eax
	ja	9f
	lea	(%rsi,%rcx),%eax
	and	$0xfff,%eax
	cmp	$0xff0,%eax
	jbe	7f
9:	cmpq	%rcx,%rdx
	je	1f
	movzbl	(%rdi,%rcx),%r8d
	movzbl	(%rsi,%rcx),%r9d
	mov	%r8d,%eax
	sub	%r9d,%eax
	testl	%r8d,%r8d
	je	5f
	incq	%rcx
	testl	%eax,%eax
	je	4b
	jmp	5f
1:	xor	%eax,%eax
5:	.leafepilogue
7:	vmovdqu	(%rsi,%rcx),%xmm3
	vpcmpeqb (%rdi,%rcx),%xmm3,%xmm2
	vpcmpeqb %xmm0,%xmm3,%xmm3
	vpandn	%xmm1,%xmm2,%xmm2
	vpor	%xmm3,%xmm2,%xmm2
	vpmovmskb %xmm2,%eax
	bsf	%eax,%eax
	jz	3b
	mov	%eax,%edx
	add	%rdx,%rdi
	movzbl	(%rcx,%rdi),%eax
	add	%rdx,%rsi
	movzbl	(%rcx,%rsi),%ecx
	sub	%ecx,%eax
	jmp	5b
	.endfn	strncmp$avx,globl,hidden
	.source	__FILE__
