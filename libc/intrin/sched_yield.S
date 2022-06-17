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
#include "libc/dce.h"
#include "libc/sysv/consts/nr.h"
#include "libc/macros.internal.h"
.privileged

//	Asks kernel to let other threads be scheduled.
//
//	@return	0 on success, or -1 w/ errno
sched_yield:

#if SupportsXnu()
	testb	IsXnu()
	jz	1f
	pause
	ret
#endif

#if SupportsWindows()
//	Windows Support
//
//	A value of zero, together with the bAlertable parameter set to
//	FALSE, causes the thread to relinquish the remainder of its time
//	slice to any other thread that is ready to run, if there are no
//	pending user APCs on the calling thread. If there are no other
//	threads ready to run and no user APCs are queued, the function
//	returns immediately, and the thread continues execution.
//	                                  ──Quoth MSDN
	testb	IsWindows()
	jz	1f
	push	%rbp
	mov	%rsp,%rbp
	xor	%ecx,%ecx
	xor	%edx,%edx
	ntcall	__imp_SleepEx
	xor	%eax,%eax
	pop	%rbp
	ret
#endif

#if SupportsSystemv()
//	UNIX Support
1:	mov	__NR_sched_yield,%eax
#if SupportsBsd() && SupportsLinux()
	clc
#endif
	syscall
#if SupportsBsd()
	jc	systemfive_errno
#endif
#if SupportsLinux()
	cmp	$-4095,%rax
	jae	systemfive_error
#endif
#endif

2:	ret
	.endfn	sched_yield,globl
	.previous
