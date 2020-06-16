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
#include "libc/dce.h"
#include "libc/macros.h"

/	Searches for last instance of byte in memory region.
/
/	@param	rdi points to data to search
/	@param	esi is treated as unsigned char
/	@param	rdx is byte length of rdi
/	@return	rax is address of last %sil in %rdi, or NULL
/	@note	AVX2 requires Haswell (2014+) or Excavator (2015+)
/	@asyncsignalsafe
memrchr:.leafprologue
	.profilable
#if !IsTiny()
	cmp	$32,%rdx
	jb	5f
	testb	X86_HAVE(AVX2)+kCpuids(%rip)
	jz	5f
	vmovd	%esi,%xmm0
	vpbroadcastb %xmm0,%ymm0
3:	vmovups	-32(%rdi,%rdx),%ymm1
	vpcmpeqb %ymm1,%ymm0,%ymm1
	vpmovmskb %ymm1,%eax
	lzcnt	%eax,%eax
	mov	%eax,%ecx
	sub	%rcx,%rdx
	cmp	$32,%eax
	jne	5f
	cmp	$31,%rdx
	ja	3b
	vzeroupper
#endif
5:	xor	%eax,%eax
	mov	%rdx,%rcx
6:	sub	$1,%rcx
	jb	9f
	cmp	%sil,-1(%rdi,%rdx)
	mov	%rcx,%rdx
	jne	6b
	lea	(%rdi,%rcx),%rax
9:	.leafepilogue
	.endfn	memrchr,globl
	.source	__FILE__
