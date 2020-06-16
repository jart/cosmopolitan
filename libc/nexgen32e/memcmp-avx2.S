/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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

/	Compares memory bytes.
/
/	@param	rdi is first uint8_t array
/	@param	rsi is second uint8_t array
/	@param	rdx is max bytes to consider
/	@return	unsigned char subtraction at stop index
/	@note	AVX2 requires Haswell (2014+) or Excavator (2015+)
/	@see	libc/nexgen32e/memcmp.S (for benchmarks)
/	@asyncsignalsafe
memcmp$avx2:
	.leafprologue
	.profilable
	cmp	%rsi,%rdi
	je	7f
	test	%rdx,%rdx
	jz	7f
	mov	%rdx,%r8
	shr	$5,%r8
	add	$1,%r8
	mov	$-32,%rcx
1:	add	$32,%rcx
	sub	$1,%r8
	jz	5f
	vmovdqu	(%rdi,%rcx),%ymm0
	vpcmpeqb (%rsi,%rcx),%ymm0,%ymm0
	vpmovmskb %ymm0,%eax
	sub	$0xffffffff,%eax
	jz	1b
	tzcnt	%eax,%eax
	add	%rax,%rcx
5:	cmp	%rcx,%rdx
	je	7f
	inc	%rcx
	movzbl	-1(%rdi,%rcx),%eax
	movzbl	-1(%rsi,%rcx),%r8d
	sub	%r8d,%eax
	jz	5b
	jmp	8f
7:	xor	%eax,%eax
8:	vxorps	%ymm0,%ymm0,%ymm0
	.leafepilogue
	.endfn	memcmp$avx2,globl,hidden
	.source	__FILE__
