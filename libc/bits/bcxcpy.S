/*-*- mode:asm; indent-tabs-mode:t; tab-width:8; coding:utf-8               -*-│
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

bcxcpy:	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%rbx
	xor	%ebx,%ebx
	lea	-64(%rbp),%rdx
	sub	$24,%rsp
3:	lea	(,%rbx,4),%ecx
	mov	%rsi,%rax
	shr	%cl,%rax
	and	$15,%eax
	cmp	$9,%eax
	lea	7(%rax),%ecx
	cmova	%ecx,%eax
	add	$48,%eax
	mov	%al,(%rdx,%rbx)
	add	$1,%rbx
	cmp	$16,%rbx
	jne	3b
	mov	%rdx,%rax
	lea	-48(%rbp),%rcx
	lea	15(%rdi),%rdx
4:	movzbl	(%rax),%ebx
	add	$1,%rax
	sub	$1,%rdx
	mov	%bl,1(%rdx)
	cmp	%rcx,%rax
	jne	4b
	add	$24,%rsp
	pop	%rbx
	pop	%rbp
	ret
	.endfn	bcxcpy,globl
