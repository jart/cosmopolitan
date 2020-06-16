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
#include "libc/dce.h"
#include "libc/macros.h"

/	Returns system wall time in microseconds.
/
/	@param	rdi points to timeval that receives result
/	@param	rsi receives UTC timezone if non-NULL
/	@return	always zero
/	@see	clock_gettime() for nanosecond precision
/	@see	strftime() for string formatting
gettimeofday$sysv:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	test	%rsi,%rsi
	jz	1f
	push	$0
	pop	(%rsi)
1:	xor	%esi,%esi			# no one zones this way.
	xor	%edx,%edx			# i64*mach_absolute_time
	call	__gettimeofday$sysv
#if SupportsXnu()
	testb	IsXnu()				# XNU might do %rax:%rdx
	jz	1f
	test	%rdi,%rdi
	jz	1f
	test	%rax,%rax
	jz	1f
	mov	%rax,(%rdi)
	mov	%rdx,8(%rdi)
#endif
1:	xor	%eax,%eax			# nevar fail
	pop	%rbp
	ret
	.endfn	gettimeofday$sysv,globl,hidden
