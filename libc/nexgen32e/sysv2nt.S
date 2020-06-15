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
.text.windows

/	Epilogues for calling functions w/ Microsoft x64 convention.
/
/	@param	%rax is address of windows x64 function to call
/	@param	%rsi is ignored
/	@param	%rdx,%rcx,%r8,%r9,stack are params (unlimited)
/	@param	%xmm0,%xmm1,%xmm2 are double params (limited to 3)
/	@return	is in %rax, %xmm0, or %st
/	@note	GCC 4.8+ and Clang can avoid this indirection
/	@note	thunk that jumps here must setup frame
/	@note	this is so much faster than nt2sysv()
__sysv2nt14:
	pushq	72(%rbp)
	pushq	64(%rbp)
__sysv2nt12:
	pushq	56(%rbp)
	pushq	48(%rbp)
__sysv2nt10:
	pushq	40(%rbp)
	pushq	32(%rbp)
__sysv2nt8:
	pushq	24(%rbp)
	pushq	16(%rbp)
__sysv2nt6:
	push	%r9
	push	%r8
__sysv2nt:
	mov	%rdx,%r8
	mov	%rcx,%r9
	mov	%rdi,%rcx
	mov	%rsi,%rdx
	sub	$32,%rsp
	call	*%rax
	leave
	ret
	.endfn	__sysv2nt,globl,hidden
	.endfn	__sysv2nt6,globl,hidden
	.endfn	__sysv2nt8,globl,hidden
	.endfn	__sysv2nt10,globl,hidden
	.endfn	__sysv2nt12,globl,hidden
	.endfn	__sysv2nt14,globl,hidden
	.yoink	__FILE__
