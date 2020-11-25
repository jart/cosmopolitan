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
.text.windows

/	Translates function call from code built w/ MS-style compiler.
/
/	This wraps WinMain() and callback functions passed to Win32 API.
/	Please note an intermediary jump slot is needed to set %rax.
/
/	@param	%rax is function address
/	@param	%rcx,%rdx,%r8,%r9
/	@return	%rax,%xmm0
/	@note	slower than __sysv2nt
/	@see	NT2SYSV() macro
__nt2sysv:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	sub	$0x100,%rsp
	push	%rbx
	push	%rdi
	push	%rsi
	pushf
	ezlea	_base,bx
	lea	-0x80(%rbp),%rdi
	call	savexmm
	mov	%rcx,%rdi
	mov	%rdx,%rsi
	mov	%r8,%rdx
	mov	%r9,%rcx
	call	*%rax
	lea	-0x80(%rbp),%rdi
	call	loadxmm
	popf
	pop	%rsi
	pop	%rdi
	pop	%rbx
	leave
	ret
	.endfn	__nt2sysv,globl,hidden
	.source	__FILE__
