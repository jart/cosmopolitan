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
.privileged
.alignfunc

/	Returns 𝑥*𝑦, aborting on overflow.
/
/	@param	edi is int32 𝑥
/	@param	esi is int32 𝑦
/	@return	eax is 𝑥*𝑦
/	@see	-ftrapv
__mulvsi3:
	mov	%edi,%eax
	imul	%esi
	jc	1f
	ret
1:	push	%rbp
	mov	%rsp,%rbp
	call	__on_arithmetic_overflow
	pop	%rbp
	ret
	.endfn	__mulvsi3,globl
