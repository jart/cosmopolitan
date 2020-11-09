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

/	Switches stack.
/
/	@param	rdi is new rsp, passed as malloc(size) + size
/	@param	rsi is function to call in new stack space
/	@param	rdx,rcx,r8,r9 get passed as args to rsi
/	@return	rax and happens on original stack
_setstack:
	push	%rbp
	mov	%rsp,%rbp
	push	%rbx
	mov	%rsp,%rbx
	mov	%rdi,%rsp
	push	16(%rbx)
	push	8(%rbx)
	mov	%rsi,%rax
	mov	%rdx,%rdi
	mov	%rcx,%rsi
	mov	%r8,%rdx
	mov	%r9,%rcx
	call	*%rax
	mov	%rbx,%rsp
	pop	%rbx
	pop	%rbp
	ret
	.endfn	_setstack,globl,hidden
