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
.source	__FILE__

/	These function names make it a bit more obvious which signal
/	caused the crash, particularly in the GDB GUI. They're coded
/	into an array to pinch pennies on code size registering them.

kOncrashThunks:

	.org	11*0
oncrash_sigquit:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigquit,globl

	.org	11*1
oncrash_sigfpe:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigfpe,globl

	.org	11*2
oncrash_sigill:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigill,globl

	.org	11*3
oncrash_sigsegv:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigsegv,globl

	.org	11*4
oncrash_sigtrap:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigtrap,globl

	.org	11*5
oncrash_sigabrt:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigabrt,globl

	.org	11*6
oncrash_sigbus:
	push	%rbp
	mov	%rsp,%rbp
	call	oncrash
	pop	%rbp
	ret
	.endfn	oncrash_sigbus,globl

	.endobj	kOncrashThunks,globl
