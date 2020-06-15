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

tinymath_frexp:
	.leafprologue
	push	%rbx
	push	%rdx
	mov	%rdi,%rbx
	movq	%xmm0,%rax
	shr	$52,%rax
	and	$0x7ff,%eax
	jne	3f
	xorps	%xmm1,%xmm1
	ucomisd	%xmm1,%xmm0
	jp	1f
	je	2f
1:	mulsd	6f(%rip),%xmm0
	mov	%rbx,%rdi
	call	frexp
	subl	$64,(%rbx)
	jmp	5f
2:	movl	$0,(%rdi)
	jmp	5f
3:	cmp	$0x7ff,%eax
	je	5f
	movq	%xmm0,%rdx
	sub	$0x3fe,%eax
	mov	%eax,(%rdi)
	movabs	$-9218868437227405313,%rax
	and	%rax,%rdx
	mov	$511,%eax
	sal	$53,%rax
	or	%rax,%rdx
	movq	%rdx,%xmm0
5:	pop	%rax
	pop	%rbx
	.leafepilogue
	.endfn	tinymath_frexp,globl
	.alias	tinymath_frexp,frexp

	.rodata.cst8
6:	.long	0,0x43f00000
