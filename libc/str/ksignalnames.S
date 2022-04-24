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

	.macro	.e e s
	.long	\e - kSignalNames
	.long	1f - kSignalNames
	.rodata.str1.1
1:	.string	"\s"
	.previous
	.endm

	.section .rodata
	.align 4
	.underrun
kSignalNames:
	.e	SIGHUP,"HUP"
	.e	SIGINT,"INT"
	.e	SIGQUIT,"QUIT"
	.e	SIGILL,"ILL"
	.e	SIGTRAP,"TRAP"
	.e	SIGABRT,"ABRT"
	.e	SIGBUS,"BUS"
	.e	SIGFPE,"FPE"
	.e	SIGKILL,"KILL"
	.e	SIGUSR1,"USR1"
	.e	SIGSEGV,"SEGV"
	.e	SIGUSR2,"USR2"
	.e	SIGPIPE,"PIPE"
	.e	SIGALRM,"ALRM"
	.e	SIGTERM,"TERM"
	.e	SIGSTKFLT,"STKFLT"
	.e	SIGCHLD,"CHLD"
	.e	SIGCONT,"CONT"
	.e	SIGSTOP,"STOP"
	.e	SIGTSTP,"TSTP"
	.e	SIGTTIN,"TTIN"
	.e	SIGTTOU,"TTOU"
	.e	SIGURG,"URG"
	.e	SIGXCPU,"XCPU"
	.e	SIGXFSZ,"XFSZ"
	.e	SIGVTALRM,"VTALRM"
	.e	SIGPROF,"PROF"
	.e	SIGWINCH,"WINCH"
	.e	SIGIO,"IO"
	.e	SIGSYS,"SYS"
	.e	SIGINFO,"INFO"
	.e	SIGRTMAX,"RTMAX"
	.e	SIGRTMIN,"RTMIN"
	.e	SIGEMT,"EMT"
	.e	SIGPWR,"PWR"
	.long	-123
	.endobj	kSignalNames,globl,hidden
	.overrun
