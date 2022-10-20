/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/dce.h"
#include "libc/intrin/strace.internal.h"
#include "libc/thread/tls.h"
#include "libc/macros.internal.h"

//	Forks process without copying page tables.
//
//	This is the same as fork() except it's optimized for the case
//	where the caller invokes execve() immediately afterwards. You
//	can also call functions like close(), dup2(), etc. Call _exit
//	but don't call exit. Look for vforksafe function annotations,
//	For example pthread mutexes are @vforksafe because they don't
//	do anything in a vfork()'d child process. TLS memory must not
//	be disabled (it's enabled by default) since vfork() needs it.
//
//	What makes vfork() dangerous is that any changes to memory in
//	the child process can happen in the parent too. The exception
//	to this rule is `errno` which is saved/restored in a register
//	by this implementation. However, despite its dangers, vfork's
//	performance is irresistible and wonderous to behold. If safer
//	code is desired, consider posix_spawn() which uses vfork().
//
//	Do not make the assumption that the parent is suspended until
//	the child terminates since this uses the raw fork system call
//	on Windows, OpenBSD, and MacOS. In that case the process will
//	proceed without blocking the parent; however, the `__vforked`
//	variable is still set to true in the child, so lock functions
//	won't do anything, and other functions shall change behavior.
//	This ensures that, even if the operating system does not give
//	us the performance of vfork(), we'll still be able to cut out
//	the libc overhead, e.g. pthread_atfork().
//
//	@return	pid of child process or 0 if forked process
//	@returnstwice
//	@threadsafe
//	@vforksafe
vfork:
#if !IsTiny()
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	call	__require_tls
#ifdef SYSDEBUG
	ezlea	.Llog,di
	call	__stracef
#endif
	pop	%rbp
#endif
	mov	%fs:0,%r9		# get thread information block
#if SupportsWindows()
	testb	IsWindows()
	jnz	6f			# and we're lucky to have that
#endif
#ifdef __SANITIZE_ADDRESS__
	jmp	5f			# TODO: asan and vfork don't mix?
#endif
#if SupportsXnu()
	testb	IsXnu()
	jnz	5f
#endif
#if SupportsOpenbsd()
	testb	IsOpenbsd()
	jnz	5f			# fake vfork plus msyscall issues
#endif
	mov	0x3c(%r9),%r8d		# avoid question of @vforksafe errno
	pop	%rsi			# saves return address in a register
	mov	__NR_vfork(%rip),%eax
#if SupportsBsd()
	clc
#endif
	syscall
#if SupportsBsd()
	jnc	0f
	neg	%rax
0:
#endif
	push	%rsi			# note it happens twice in same page
	cmp	$-4095,%eax
	jae	systemfive_error
	mov	%r8d,0x3c(%r9)		# restore errno
1:	test	%eax,%eax
	jnz	.Lpar
.Lchi:	orb	$TIB_FLAG_VFORKED,0x40(%r9)
	ret
.Lpar:	andb	$~TIB_FLAG_VFORKED,0x40(%r9)
	ret
#if SupportsXnu() || SupportsOpenbsd() || defined(__SANITIZE_ADDRESS__)
5:	push	%rbp
	mov	%rsp,%rbp
	push	%r9
	push	%r9
	call	sys_fork
	pop	%r9
	pop	%r9
	pop	%rbp
	jmp	1b
#endif
#if SupportsWindows()
6:	push	%rbp
	mov	%rsp,%rbp
	push	%r9
	push	%r9
	xor	%edi,%edi		# dwCreationFlags
	call	sys_fork_nt
	pop	%r9
	pop	%r9
	pop	%rbp
	jmp	1b
#endif
	.endfn	vfork,globl

#ifdef SYSDEBUG
	.rodata.str1.1
.Llog:	.ascii	STRACE_PROLOGUE
	.asciz	"vfork()\n"
	.previous
#endif /* DEBUGSYS */
