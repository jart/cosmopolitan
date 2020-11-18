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
#include "libc/runtime/internal.h"
#include "libc/macros.h"
.text.startup
.source	__FILE__

/	Calls global initialization functions.
/
/	@param	r12 is argc
/	@param	r13 is argv
/	@param	r14 is environ
/	@param	r15 is auxv
_construct:
	push	%rbp
	mov	%rsp,%rbp
	ezlea	__init_array_start,ax		# static ctors in forward order
	.weak	__init_array_start		# could be called multiple times
	ezlea	__init_array_end,cx		# idempotency recommended
	.weak	__init_array_end		# @see ape/ape.lds
1:	cmp	%rax,%rcx
	je	2f
	push	%rax
	push	%rcx
	mov	%r12,%rdi
	mov	%r13,%rsi
	mov	%r14,%rdx
	mov	%r15,%rcx
	call	*(%rax)
	pop	%rcx
	pop	%rax
	add	$8,%rax
	jmp	1b
2:	pop	%rbp
	ret
	.endfn	_construct,globl
