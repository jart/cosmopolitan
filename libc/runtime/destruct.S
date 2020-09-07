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
.text.exit
.source	__FILE__

/	Calls linker registered finalization functions.
/	@note	functions are called in reverse order
_destruct:
	push	%rbp
	mov	%rsp,%rbp
	ezlea	__fini_array_start,cx
	.weak	__fini_array_start
	ezlea	__fini_array_end,ax
	.weak	__fini_array_end
	cmp	%rax,%rcx
	je	2f
1:	sub	$8,%rax
	push	%rax
	push	%rcx
	call	*(%rax)
	pop	%rcx
	pop	%rax
	cmp	%rax,%rcx
	jne	1b
2:	pop	%rbp
	ret
	.endfn	_destruct,globl
