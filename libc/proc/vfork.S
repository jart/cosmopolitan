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
#include "libc/intrin/strace.internal.h"
#include "libc/thread/tls.h"
#include "libc/macros.internal.h"

//	Forks process without copying page tables.
//
//	This function lets a process spawn another process without
//	copying the page tables. The parent process gets suspended
//	until the child calls either execve() or _Exit(), and they
//	share memory during that time. That's at least how we want
//	vfork() to work. Support for these behaviors is patchy. It
//	is also error-prone to use this function in an environment
//	with signal handlers and threads. The best way to use this
//	is by calling posix_spawn() which works around the dangers
//	and determines at runtime the best way to pass error codes
//
//	@return	pid of child process or 0 if forked process
//	@returnstwice
//	@vforksafe
	.ftrace1
vfork:
	.ftrace2

#ifdef __SANITIZE_ADDRESS__
	jmp	fork
#endif

#ifdef __x86_64__

#if SupportsWindows()
//	these platforms disagree with vfork
	testb	$_HOSTXNU|_HOSTOPENBSD|_HOSTWINDOWS,__hostos(%rip)
	jnz	fork
#endif

#if !IsTiny()
	push	%rbp
	mov	%rsp,%rbp
#if SYSDEBUG
	ezlea	.Llog,di
	call	__stracef
#endif
	pop	%rbp
#endif
	mov	%fs:0,%r9		// get thread information block
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
	jae	systemfive_error
	mov	%r8d,0x3c(%r9)		// restore errno
1:	test	%eax,%eax
	jnz	.Lpar
.Lchi:	orb	$TIB_FLAG_VFORKED,0x40(%r9)
	ret
.Lpar:	andb	$~TIB_FLAG_VFORKED,0x40(%r9)
	ret

#elif defined(__aarch64__)

	adrp	x8,__hostos
	ldr	w8,[x8,#:lo12:__hostos]
	tbz	x8,3,1f			// bit 3 is xnu
	b	fork			// which doesn't support vfork()
1:

#if SYSDEBUG
	stp	x29,x30,[sp,-16]!
	adrp	x0,.Llog
	add	x0,x0,:lo12:.Llog
	mov	x29,sp
	bl	__stracef
	ldp	x29,x30,[sp],16
#endif

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
	sub	x1,x28,#192		// sizeof(CosmoTib)
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
