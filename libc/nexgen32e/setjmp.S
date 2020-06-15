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

/	Saves caller CPU state to cacheline.
/
/	@param	rdi points to jmp_buf
/	@return	rax 0 when set and !0 when longjmp'd
/	@returnstwice
/	@assume	system five nexgen32e abi conformant
/	@note	code built w/ microsoft abi compiler can't call this
/	@see	longjmp(), gclongjmp()
setjmp:	lea	8(%rsp),%rax
	mov	%rax,(%rdi)
	mov	%rbx,8(%rdi)
	mov	%rbp,16(%rdi)
	mov	%r12,24(%rdi)
	mov	%r13,32(%rdi)
	mov	%r14,40(%rdi)
	mov	%r15,48(%rdi)
	mov	(%rsp),%rax
	mov	%rax,56(%rdi)
	xor	%eax,%eax
	ret
	.endfn	setjmp,globl
	.yoink	__FILE__
