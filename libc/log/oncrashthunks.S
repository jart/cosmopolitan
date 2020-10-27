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

/	These function names make it a bit more obvious which signal
/	caused the crash, particularly in the GDB GUI. They're coded
/	into an array to pinch pennies on code size registering them.

__oncrash_thunks:

	.org	11*0
__oncrash_sigquit:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigquit,globl

	.org	11*1
__oncrash_sigfpe:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigfpe,globl

	.org	11*2
__oncrash_sigill:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigill,globl

	.org	11*3
__oncrash_sigsegv:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigsegv,globl

	.org	11*4
__oncrash_sigtrap:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigtrap,globl

	.org	11*5
__oncrash_sigabrt:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigabrt,globl

	.org	11*6
__oncrash_sigbus:
	push	%rbp
	mov	%rsp,%rbp
	call	__oncrash
	pop	%rbp
	ret
	.endfn	__oncrash_sigbus,globl

	.endobj	__oncrash_thunks,globl
