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
.privileged

/	Loads previously saved processor state.
/
/	@param	rdi points to the jmp_buf
/	@param	esi is returned by setjmp() invocation (coerced nonzero)
/	@noreturn
/	@assume	system five nexgen32e abi conformant
/	@note	code built w/ microsoft abi compiler can't call this
/	@see	gclongjmp() unwinds gc() destructors
longjmp:mov	%esi,%eax
	test	%eax,%eax
	jnz	1f
	inc	%eax
1:	mov	(%rdi),%rsp
	mov	8(%rdi),%rbx
	mov	16(%rdi),%rbp
	mov	24(%rdi),%r12
	mov	32(%rdi),%r13
	mov	40(%rdi),%r14
	mov	48(%rdi),%r15
	jmp	*56(%rdi)
	.endfn	longjmp,globl
	.yoink	__FILE__
