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

diagnose_syscall:
	push	%rbp
	mov	%rsp,%rbp
	push	%rbx
	push	%r12
	push	%r13
	push	%r14
	push	%r15

	mov	$0x7fffffff,%eax
	add	$4,%eax				# set sf/of/pf

	mov	%rdi,%rax			# nr
	mov	%rsi,%rdi			# arg 1
	mov	%rdx,%rsi			# arg 2
	mov	%rcx,%rdx			# arg 3
	mov	%r8,%r10			# arg 4
	mov	%r9,%r8				# arg 5
	mov	16(%rbp),%r9			# arg 6
	push	24(%rbp)			# arg 7
	push	%rax				# fake ret addr
	mov	32(%rbp),%r12			# ucontext before
	mov	40(%rbp),%r13			# ucontext after
	xor	%ecx,%ecx
	xor	%r11d,%r11d
	mov	$0x5555555555555555,%r11
	mov	$0x5555555555555555,%r14
	mov	$0x5555555555555555,%r15
	mov	$0x5555555555555555,%rbx

//	save machine state before system call
	pushf
	pop	176(%r12)
	mov	%r8,40(%r12)
	mov	%r9,48(%r12)
	mov	%r10,56(%r12)
	mov	%r11,64(%r12)
	mov	%r12,72(%r12)
	mov	%r13,80(%r12)
	mov	%r14,88(%r12)
	mov	%r15,96(%r12)
	mov	%rdi,104(%r12)
	mov	%rsi,112(%r12)
	mov	%rbp,120(%r12)
	mov	%rbx,128(%r12)
	mov	%rdx,136(%r12)
	mov	%rax,144(%r12)
	mov	%rcx,152(%r12)
	push	%rbx
	lea	320(%r12),%rbx
	mov	%rbx,224(%r12)			# set fpregs ptr
	pop	%rbx

	syscall

//	save machine state after system call
	pushf
	pop	176(%r13)
	mov	%r8,40(%r13)
	mov	%r9,48(%r13)
	mov	%r10,56(%r13)
	mov	%r11,64(%r13)
	mov	%r12,72(%r13)
	mov	%r13,80(%r13)
	mov	%r14,88(%r13)
	mov	%r15,96(%r13)
	mov	%rdi,104(%r13)
	mov	%rsi,112(%r13)
	mov	%rbp,120(%r13)
	mov	%rbx,128(%r13)
	mov	%rdx,136(%r13)
	mov	%rax,144(%r13)
	mov	%rcx,152(%r13)
	push	%rbx
	lea	320(%r13),%rbx
	mov	%rbx,224(%r13)			# set fpregs ptr
	pop	%rbx

	pop	%r13
	pop	%r13

	pop	%r15
	pop	%r14
	pop	%r13
	pop	%r12
	pop	%rbx
	pop	%rbp
	ret
	.endfn	diagnose_syscall,globl
