/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/macros.internal.h"
.privileged

//	Invokes SYSCALL for libfatal forceinline asm() routines.
//
//	@param	rax is ordinal
//	@param	rdi is arg1
//	@param	rsi is arg2
//	@param	rdx is arg3
//	@param	rcx is arg4
//	@param	r8  is arg5
//	@param	r9  is arg6
//	@param	rsp may contain more args
//	@return	rdx:rax where rax holds -errno on error
//	@clob	rax,rdx,memory,cc
__syscall__:
	mov	%rcx,.Lrcx(%rip)
	mov	%rdi,.Lrdi(%rip)
	mov	%rsi,.Lrsi(%rip)
	mov	%r8,.Lr8(%rip)
	mov	%r9,.Lr9(%rip)
	mov	%r10,.Lr10(%rip)
	mov	%r11,.Lr11(%rip)
	mov	%rcx,%r10
	clc
	syscall
	jnc	1f
	neg	%rax
1:	mov	.Lrcx(%rip),%rcx
	mov	.Lrdi(%rip),%rdi
	mov	.Lrsi(%rip),%rsi
	mov	.Lr8(%rip),%r8
	mov	.Lr9(%rip),%r9
	mov	.Lr10(%rip),%r10
	mov	.Lr11(%rip),%r11
	ret
	.endfn	__syscall__,globl,hidden

	.bss
	.align	8
.Lrcx:	.quad	0			# clobbered by syscall
.Lrdi:	.quad	0			# just in case
.Lrsi:	.quad	0			# just in case
.Lr8:	.quad	0			# freebsd bug?
.Lr9:	.quad	0			# just in case
.Lr10:	.quad	0			# just in case
.Lr11:	.quad	0			# clobbered by syscall
