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

/	Creates bit mask of which bytes are the same.
/
/	@param	%rdi points to bit mask (write-only)
/	@param	%rsi points to first buffer (read-only)
/	@param	%rdx points to second buffer (read-only)
/	@param	%rcx is byte length of both %rsi and %rdx
/	@return	%rax is set to %rdi
/	@note	buffers should be 128-byte aligned
memeqmask:
	.leafprologue
	xor	%eax,%eax
	test	%ecx,%ecx
	jz	1f
	shr	$3,%ecx
0:	movdqa	(%rsi,%rax,8),%xmm0
	movdqa	16(%rsi,%rax,8),%xmm1
	pcmpeqb	(%rdx,%rax,8),%xmm0
	pcmpeqb	16(%rdx,%rax,8),%xmm1
	vpmovmskb %xmm0,%r8d
	vpmovmskb %xmm1,%r9d
	mov	%r8w,(%rdi,%rax)
	mov	%r9w,2(%rdi,%rax)
	add	$4,%eax
	cmp	%ecx,%eax
	jb	0b
1:	mov	%rdi,%rax
	.leafepilogue
	.endfn	memeqmask,globl
	.yoink	__FILE__
