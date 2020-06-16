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

__fpclassifyf:
	.leafprologue
	movd	%xmm0,%edx
	movd	%xmm0,%eax
	shr	$23,%eax
	and	$255,%eax
	je	7f
	cmp	$255,%eax
	je	8f
	mov	$FP_NORMAL,%eax
	jmp	1f
7:	add	%edx,%edx
	je	5f
	mov	$FP_SUBNORMAL,%eax
	jmp	1f
5:	mov	$FP_ZERO,%eax
	jmp	1f
8:	sal	$9,%edx
	sete	%al
	movzbl	%al,%eax
1:	.leafepilogue
	.endfn	__fpclassifyf,globl
