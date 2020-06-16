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
#include "libc/math.h"
#include "libc/macros.h"
.source	__FILE__

__fpclassifyl:
	push	%rbp
	mov	%rsp,%rbp
	mov	24(%rbp),%rax
	mov	16(%rbp),%rdx
	and	$0x7fff,%ax
	mov	%rdx,%rcx
	shr	$63,%rcx
	movzwl	%ax,%esi
	or	%ecx,%esi
	jne	2f
	cmp	$1,%rdx
	sbb	%eax,%eax
	add	$3,%eax
	jmp	1f
2:	cmp	$0x7fff,%ax
	jne	4f
	xor	%eax,%eax
	test	%rcx,%rcx
	je	1f
	xor	%eax,%eax
	add	%rdx,%rdx
	sete	%al
	jmp	1f
4:	mov	%ecx,%eax
	neg	%eax
	and	$FP_NORMAL,%eax
1:	pop	%rbp
	ret
	.endfn	__fpclassifyl,globl
