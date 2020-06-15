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

/	Thunks double(*fn)(double,double) -> long double fn.
/
/	@param	%xmm0[0] contains double param
/	@return	%xmm0[0] contains double result
/	@note	100% negligible overhead
_d2ld2:	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	movsd	%xmm0,-32(%rbp)
	fldl	-32(%rbp)
	fstpt	-32(%rbp)
	movsd	%xmm1,-16(%rbp)
	fldl	-16(%rbp)
	fstpt	-16(%rbp)
	call	*%rax
	fstpl	-16(%rbp)
	movsd	-16(%rbp),%xmm0
	leave
	ret
	.endfn	_d2ld2,globl,hidden
