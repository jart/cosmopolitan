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

/	Jump table for palignr() with non-constexpr immediate parameter.
/
/	@note	needs ssse3 cf. prescott c. 2004 cf. bulldozer c. 2011
/	@see	palignr()
	.align	8
__palignrs:
  	palignr	$0,%xmm1,%xmm0
	ret
	nop
  	palignr	$1,%xmm1,%xmm0
	ret
	nop
  	palignr	$2,%xmm1,%xmm0
	ret
	nop
  	palignr	$3,%xmm1,%xmm0
	ret
	nop
  	palignr	$4,%xmm1,%xmm0
	ret
	nop
  	palignr	$5,%xmm1,%xmm0
	ret
	nop
  	palignr	$6,%xmm1,%xmm0
	ret
	nop
  	palignr	$7,%xmm1,%xmm0
	ret
	nop
  	palignr	$8,%xmm1,%xmm0
	ret
	nop
  	palignr	$9,%xmm1,%xmm0
	ret
	nop
  	palignr	$10,%xmm1,%xmm0
	ret
	nop
  	palignr	$11,%xmm1,%xmm0
	ret
	nop
  	palignr	$12,%xmm1,%xmm0
	ret
	nop
  	palignr	$13,%xmm1,%xmm0
	ret
	nop
  	palignr	$14,%xmm1,%xmm0
	ret
	nop
  	palignr	$15,%xmm1,%xmm0
	ret
	nop
  	palignr	$16,%xmm1,%xmm0
	ret
	nop
  	palignr	$17,%xmm1,%xmm0
	ret
	nop
  	palignr	$18,%xmm1,%xmm0
	ret
	nop
  	palignr	$19,%xmm1,%xmm0
	ret
	nop
  	palignr	$20,%xmm1,%xmm0
	ret
	nop
  	palignr	$21,%xmm1,%xmm0
	ret
	nop
  	palignr	$22,%xmm1,%xmm0
	ret
	nop
  	palignr	$23,%xmm1,%xmm0
	ret
	nop
  	palignr	$24,%xmm1,%xmm0
	ret
	nop
  	palignr	$25,%xmm1,%xmm0
	ret
	nop
  	palignr	$26,%xmm1,%xmm0
	ret
	nop
  	palignr	$27,%xmm1,%xmm0
	ret
	nop
  	palignr	$28,%xmm1,%xmm0
	ret
	nop
  	palignr	$29,%xmm1,%xmm0
	ret
	nop
  	palignr	$30,%xmm1,%xmm0
	ret
	nop
  	palignr	$31,%xmm1,%xmm0
	ret
	.if	. - __palignrs != 8 * 32 - 1
	.error	"bad assemblage"
	.endif
	.endfn	__palignrs,globl
