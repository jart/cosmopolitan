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
.source	__FILE__

/	Returns 𝑥 × 2ʸ.
/
/	@param	𝑥 is float passed in %xmm0
/	@param	𝑦 is exponent via %edi
tinymath_scalbnf:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	push	%rax
	movss	%xmm0,-4(%rbp)
	flds	-4(%rbp)
	movl	%edi,-4(%rbp)
	fildl	-4(%rbp)
	fxch	%st(1)
	fscale
	fstp	%st(1)
	fstps	-4(%rbp)
	movss	-4(%rbp),%xmm0
	leave
	ret
	.endfn	tinymath_scalbnf,globl
	.alias	tinymath_scalbnf,scalbnf
