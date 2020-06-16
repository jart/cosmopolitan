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

__fpclassify:
	.leafprologue
	movd	%xmm0,%rax
	movd	%xmm0,%rdx
	shr	$52,%rax
	mov	%eax,%ecx
	and	$0x7ff,%ecx
	jne	2f
	add	%rdx,%rdx
	cmp	$1,%rdx
	sbb	%eax,%eax
	add	$3,%eax
	jmp	1f
2:	mov	$FP_NORMAL,%eax
	cmp	$0x7ff,%ecx
	jne	1f
	xor	%eax,%eax
	sal	$12,%rdx
	sete	%al
1:	.leafepilogue
	.endfn	__fpclassify,globl
