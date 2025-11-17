/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/intrin/strace.h"
#include "libc/thread/tls.h"
#include "libc/macros.h"

//	Forks process the fast way.
//
//	The vfork() function creates a new child process that shares the same
//	address space as the parent. The goal's to let you spawn a process in
//	a way that's scalable across threads.
//
//	MacOS, OpenBSD, QEMU, and WSL v1 do not support a true vfork() system
//	call. On such systems, this wrapper will call sys_fork() and the user
//	shouldn't assume the shared memory trick will always be available and
//	should just rely on vfork() for its performance benefits.
//
//	@return process id of child, 0 if child, or -1 w/ errno
	.ftrace1
vfork:
	.ftrace2

#ifdef __x86_64__

#if SupportsWindows()
	testb	IsWindows()
	jz	1f
	mov	%rbp,%rdx
	mov	(%rsp),%rcx
	lea	8(%rsp),%rax
	push	%rbp
	mov	%rsp,%rbp
	sub	$64,%rsp
	mov	%rsp,%rdi
	mov	%rax,(%rdi)
	mov	%rbx,8(%rdi)
	mov	%rdx,16(%rdi)
	mov	%r12,24(%rdi)
	mov	%r13,32(%rdi)
	mov	%r14,40(%rdi)
	mov	%r15,48(%rdi)
	mov	%rcx,56(%rdi)
	call	sys_vfork_nt
1:
#endif

	call	__has_vfork
	test	%eax,%eax
	jz	sys_fork

#if !IsTiny()
	push	%rbp
	mov	%rsp,%rbp
#if SYSDEBUG
	ezlea	.Llog,di
	call	__stracef
#endif
	pop	%rbp
#endif
	mov	%fs:0x30,%r9		// get thread information block
	mov	0x3c(%r9),%r8d		// avoid question of @vforksafe errno
	pop	%rsi			// saves return address in a register
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
	push	%rsi			// note it happens twice in same page
	cmp	$-4095,%eax
	jae	systemfive_errno
	mov	%r8d,0x3c(%r9)		// restore errno
1:	test	%eax,%eax
	jnz	.Lpar
.Lchi:	orb	$TIB_FLAG_VFORKED,0x40(%r9)
	ret
.Lpar:	andb	$~TIB_FLAG_VFORKED,0x40(%r9)
	ret

#elif defined(__aarch64__)

	stp	x29,x30,[sp,-16]!
	bl	__has_vfork
	ldp	x29,x30,[sp],16
	cbnz	w0,1f
	b	sys_fork
1:

#if SYSDEBUG
	stp	x29,x30,[sp,-16]!
	adrp	x0,.Llog
	add	x0,x0,:lo12:.Llog
	mov	x29,sp
	bl	__stracef
	ldp	x29,x30,[sp],16
#endif

	adrp	x8,__hostos
	ldr	w8,[x8,#:lo12:__hostos]
	tbnz	w8,5,1f			// bit 5 is freebsd

	mov	x8,#220			// clone (linux)
	mov	x0,#0x4111		// SIGCHLD | CLONE_VM | CLONE_VFORK
	mov	x1,#0
	svc	0
	b	2f

1:	mov	x8,#66			// vfork (freebsd)
	svc	0			// call kernel
	bcc	2f			// jump if not carry
	neg	x0,x0			// linux style errno
2:

//	if (!rc) {
//	  __get_tls()->tib_flags |= TIB_FLAG_VFORKED;
//	} else {
//	  __get_tls()->tib_flags &= ~TIB_FLAG_VFORKED;
//	}
	sub	x1,x28,#1024		// sizeof(CosmoTib)
	ldr	x2,[x1,64]
	cbnz	x0,2f
	orr	x2,x2,#TIB_FLAG_VFORKED
1:	str	x2,[x1,64]
	b	3f
2:	and	x2,x2,#~TIB_FLAG_VFORKED
	b	1b

//	if (rc < 0) errno = -rc, rc = -1;
3:	.hidden	_sysret
	b	_sysret

#else
#error "architecture unsupported"
#endif
	.endfn	vfork,globl

#if SYSDEBUG
	.rodata.str1.1
.Llog:	.ascii	STRACE_PROLOGUE
	.asciz	"vfork()\n"
	.previous
#endif /* DEBUGSYS */
