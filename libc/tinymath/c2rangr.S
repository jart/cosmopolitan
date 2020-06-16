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
#include "ape/lib/pc.h"
#include "libc/macros.h"
.source	__FILE__

/	Computes transcedental trigonometry op w/ reactive scaling.
/
/	@param	%rdx points to op function
/	@param	everything else delegates
/	@clob	%ax
/	@see	sin,cos,tan
c2rangr:push	%rbp
	mov	%rsp,%rbp
	.profilable
	call	*%rdx
	fstsw	%ax
	test	$FPU_C2>>8,%ah
	jnz	1f
0:	pop	%rbp
	ret
1:	fldpi
	fadd	%st
	fxch	%st(1)
2:	fprem1
	fnstsw	%ax
	test	$FPU_C2>>8,%ah
	jnz	2b
	fstp	%st(1)
	call	*%rdx
	jmp	0b
	.endfn	c2rangr,globl,hidden
