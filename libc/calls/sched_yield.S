/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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

//	Relinquishes scheduled quantum.
//
//	@return	0 on success, or -1 w/ errno
	.ftrace1
sched_yield:
	.ftrace2
#ifdef __x86_64__
	push	%rbp
	mov	%rsp,%rbp
	xor	%eax,%eax
	mov	__hostos(%rip),%dl

#if SupportsMetal()
	testb	$_HOSTMETAL,%dl
	jnz	9f
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
	testb	$_HOSTWINDOWS,%dl
	jz	1f
	xor	%ecx,%ecx
	xor	%edx,%edx
	ntcall	__imp_SleepEx
	xor	%eax,%eax
	jmp	9f
1:
#endif

#if SupportsSystemv()
//	On XNU we polyfill sched_yield() using sleep() which'll
//	be polyfilled using select() with a zero timeout, which
//	means to wait zero microseconds and then returns a zero
//	and this hopefully will give other threads a chance too
//	XNU has a special version we use called select_nocancel
//
//	"If the readfds, writefds, and errorfds arguments are
//	 all null pointers and the timeout argument is not a
//	 null pointer, the pselect() or select() function shall
//	 block for the time specified, or until interrupted by
//	 a signal." ──Quoth IEEE 1003.1-2017 §functions/select
//
//	On other platforms, sched_yield() takes no arguments.
	push	$0				// timeout.tv_usec
	push	$0				// timeout.tv_sec
	xor	%edi,%edi			// nfds
	xor	%esi,%esi			// readfds
	xor	%edx,%edx			// writefds
	xor	%r10d,%r10d			// exceptfds
	mov	%rsp,%r8			// timeout
	mov	__NR_sched_yield,%eax		// ordinal
	clc					// linux
	syscall
//	It should not be possible for this to fail so we don't
//	bother going through the errno ritual. If this somehow
//	fails a positive or negative errno might get returned.
#endif

9:	leave
	ret

#elif defined(__aarch64__)

	stp	x29,x30,[sp,-32]!
	mov	x29,sp
	mov	x3,0
	mov	x2,0
	add	x4,sp,16
	mov	x1,0
	mov	w0,0
	stp	xzr,xzr,[sp,16]
	mov	x8,#0x7c			// sched_yield() for gnu/systemd
	mov	x16,#0x5d			// select(0,0,0,0,&blah) for xnu
	svc	0
	ldp	x29,x30,[sp],32
	ret

#else
#error "arch unsupported"
#endif
	.endfn	sched_yield,globl
	.previous
