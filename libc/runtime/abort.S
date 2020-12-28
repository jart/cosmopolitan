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
#include "libc/runtime/internal.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/nr.h"
#include "libc/macros.h"
.real
.source	__FILE__

/	Terminates program abnormally.
/
/	This function first tries to trigger your SIGABRT handler. If
/	there isn't one or execution resumes, then abort() terminates
/	the program using an escalating variety methods of increasing
/	brutality.
/
/	@forcealignargpointer
/	@asyncsignalsafe
/	@noreturn
abort:	push	%rbp
	mov	%rsp,%rbp
	and	$-16,%rsp
	sub	$16,%rsp
	testb	IsWindows()
	jnz	2f
	mov	SIG_SETMASK,%edi
	mov	%rsp,%rsi
	push	$0xffffffffffffffdf		# all bits blocked but SIGABRT
	push	$0xffffffffffffffff		# assumes von neum. arithmetic
	pop	8(%rsi)
	pop	(%rsi)
	xor	%edx,%edx			# don't care about old sigmask
	pushpop	4*4,%r10			# sizeof(sigset_t) for systemd
	mov	__NR_sigprocmask,%eax		# sigprocmask$sysv is hookable
	syscall
	mov	__NR_getpid,%eax
	syscall
	mov	%eax,%edi
	mov	SIGABRT,%esi
	mov	__NR_kill,%eax
	syscall					# avoid hook and less bt noise
2:	mov	$134,%edi			# exit(128+SIGABRT) [bash-ism]
	call	_Exit
	.endfn	abort,globl,protected
