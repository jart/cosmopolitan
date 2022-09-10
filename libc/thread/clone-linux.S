/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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

//	Invokes clone() system call on GNU/Systemd.
//
//	@param	rdi is flags
//	@param	rsi is top of stack
//	@param	rdx is ptid
//	@param	rcx is ctid
//	@param	r8 is tls
//	@param	r9 is func(void*,int)→int
//	@param	8(rsp) is arg
//	@return	tid of child on success, or -1 w/ errno
sys_clone_linux:
	push	%rbp
	mov	%rsp,%rbp
	push	%rbx
	mov	%rcx,%r10
	mov	16(%rbp),%rbx
	mov	$56,%eax		# __NR_clone
	syscall
	test	%rax,%rax
	jz	2f
	cmp	$-4095,%rax
	jae	1f
0:	pop	%rbx
	pop	%rbp
	ret
1:	call	systemfive_error
	jmp	0b
2:	xor	%ebp,%ebp		# child thread
	mov	%rbx,%rdi		# arg
	mov	(%r10),%esi		# tid
	call	*%r9			# func(arg,tid)
	xchg	%eax,%edi		# func(arg,tid) → exitcode
	mov	$60,%eax		# __NR_exit(exitcode)
	syscall
	.endfn	sys_clone_linux,globl,hidden
