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

/	Performs raw System Five system call.
/
/	This function provides a direct path into system call support
/	that's friendly to C code, since it doesn't need an intermediate
/	thunk. It only supports arities up to six, since there's no way
/	to do more safely; this isn't a problem with Linux, although
/	certain BSD calls may not be available.
/
/	@param	%rdi is system call ordinal, which isn't translated,
/		and must be correct for the underlying host system
/	@param	%rsi,%rdx,%rcx,%r8,%r9 may supply parameters 1 through 5
/	@param	sixth is optionally pushed on the stack before call
/	@return	%rax has result, or -1 w/ errno on failure
syscall:mov	%rdi,%rax
	mov	%rsi,%rdi
	mov	%rdx,%rsi
	mov	%rcx,%rdx
	mov	%r8,%rcx		# ← intended
	mov	%r9,%r8
	mov	8(%rsp),%r9
	jmp	*systemfive(%rip)
	.endfn	syscall,globl
