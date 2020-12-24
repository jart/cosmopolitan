/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
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

/	Returns length of NUL-terminated string.
/
/	@param	rdi is non-null NUL-terminated string pointer
/	@return	rax is number of bytes (excluding NUL)
/	@clob	ax,dx,cx,xmm3,xmm4
/	@note	h/t agner fog
/	@asyncsignalsafe
strlen:	.leafprologue
	.profilable
	mov	%rdi,%rax
	mov	%edi,%ecx
	and	$15,%ecx
	and	$-16,%rax
	pxor	%xmm4,%xmm4
	movdqa	(%rax),%xmm3
	pcmpeqb	%xmm4,%xmm3
	pmovmskb %xmm3,%edx
	shr	%cl,%edx
	shl	%cl,%edx
	bsf	%edx,%edx
	jnz	2f
1:	lea	16(%rax),%rax
	movdqa	(%rax),%xmm3
	pcmpeqb	%xmm4,%xmm3
	pmovmskb %xmm3,%edx
	bsf	%edx,%edx
	jz	1b
2:	add	%rdx,%rax
	sub	%rdi,%rax
	.leafepilogue
	.endfn	strlen,globl
