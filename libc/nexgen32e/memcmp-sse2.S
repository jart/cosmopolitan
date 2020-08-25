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

/	Compares memory.
/
/	@param	rdi is first uint8_t array
/	@param	rsi is second uint8_t array
/	@param	rdx is max bytes to consider
/	@return	unsigned char subtraction at stop index
/	@asyncsignalsafe
memcmp$sse2:
	.leafprologue
	.profilable
	cmp	%rsi,%rdi
	je	7f
	test	%rdx,%rdx
	jz	7f
	mov	%rdx,%r8
	shr	$4,%r8
	add	$1,%r8
	mov	$-16,%rcx
1:	add	$16,%rcx
	sub	$1,%r8
	jz	5f
	movdqu	(%rdi,%rcx),%xmm0
	movdqu	(%rsi,%rcx),%xmm1
	pcmpeqb %xmm1,%xmm0
	pmovmskb %xmm0,%eax
	sub	$0xffff,%eax
	jz	1b
	bsf	%eax,%eax
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
8:	.leafepilogue
	.endfn	memcmp$sse2,globl,hidden
	.source	__FILE__
