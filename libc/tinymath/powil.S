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

/	Returns 𝑥^𝑦.
/
/	@param	𝑥 is long double passed on stack
/	@param	𝑦 is int passed in %edi
/	@return	%st
tinymath_powil:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$32,%rsp
	mov	%edi,-4(%rbp)
	fildl	-4(%rbp)
	fstpt	(%rsp)
	push	16+8(%rbp)
	push	16+0(%rbp)
	call	tinymath_powl
	leave
	ret
	.endfn	tinymath_powil,globl
	.alias	tinymath_powil,powil
