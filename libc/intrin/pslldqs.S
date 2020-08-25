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

/	Jump table for pslldq() with non-constexpr immediate parameter.
	.align	8
__pslldqs:
  	pslldq	$0,%xmm0
	ret
	nop
	nop
  	pslldq	$1,%xmm0
	ret
	nop
	nop
  	pslldq	$2,%xmm0
	ret
	nop
	nop
  	pslldq	$3,%xmm0
	ret
	nop
	nop
  	pslldq	$4,%xmm0
	ret
	nop
	nop
  	pslldq	$5,%xmm0
	ret
	nop
	nop
  	pslldq	$6,%xmm0
	ret
	nop
	nop
  	pslldq	$7,%xmm0
	ret
	nop
	nop
  	pslldq	$8,%xmm0
	ret
	nop
	nop
  	pslldq	$9,%xmm0
	ret
	nop
	nop
  	pslldq	$10,%xmm0
	ret
	nop
	nop
  	pslldq	$11,%xmm0
	ret
	nop
	nop
  	pslldq	$12,%xmm0
	ret
	nop
	nop
  	pslldq	$13,%xmm0
	ret
	nop
	nop
  	pslldq	$14,%xmm0
	ret
	nop
	nop
  	pslldq	$15,%xmm0
	ret
	nop
	nop
  	pslldq	$16,%xmm0
	ret
	.if	. - __pslldqs != 8 * 17 - 2
	.error	"bad assemblage"
	.endif
	.endfn	__pslldqs,globl
