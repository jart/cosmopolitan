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
#include "libc/notice.inc"

/	Applies no-clobber guarantee to System Five function call.
/
/	- Reentrant
/	- Realigns stack
/	- Doesn't assume red zone
/	- Clobbers nothing (except %rax and flags)
/
/	This function may be called using an stdcall convention. It's
/	useful for files named FOO.hookabi.c and BAR.ncabi.c to make
/	calls into other parts of the system, that don't conform to the
/	same restricted ABI.
/
/	@param	six args and fn addr pushed on stack in reverse order
/	@return	%rax has function return value, and stack is cleaned up
/	@see	libc/shadowargs.hook.c for intended use case
slowcall:
	#param	%r9		# 0x40 arg6
	#param	%r8		# 0x38 arg5
	#param	%rcx		# 0x30 arg4
	#param	%rdx		# 0x28 arg3
	#param	%rsi		# 0x20 arg2
	#param	%rdi		# 0x18 arg1
	#param	%rax		# 0x10 call address
	#param			# 0x08 return address
	push	%rbp		# 0x00 parent frame
	mov	%rsp,%rbp	# ----
	push	%rdi		#-0x08
	push	%rsi		#-0x10
	push	%rdx		#-0x18
	push	%rcx		#-0x20
	push	%r8		#-0x28
	push	%r9		#-0x30
	push	%r10		#-0x38
	push	%r11		#-0x40
	mov	0x10(%rbp),%rax
	mov	0x18(%rbp),%rdi
	mov	0x20(%rbp),%rsi
	mov	0x28(%rbp),%rdx
	mov	0x30(%rbp),%rcx
	mov	0x38(%rbp),%r8
	mov	0x40(%rbp),%r9
	and	$-16,%rsp
	call	*%rax
	push	%rax
	mov	0x00(%rbp),%rax
	mov	%rax,0x38(%rbp)
	mov	0x08(%rbp),%rax
	mov	%rax,0x40(%rbp)
	pop	%rax
	lea	-0x40(%rbp),%rsp
	pop	%r11
	pop	%r10
	pop	%r9
	pop	%r8
	pop	%rcx
	pop	%rdx
	pop	%rsi
	pop	%rdi
	lea	0x38(%rbp),%rsp
	pop	%rbp
	ret
	.endfn	slowcall,globl
	.source	__FILE__
