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
#include "libc/fmt/magnumstrs.internal.h"
#include "libc/macros.internal.h"

	.macro	.e e s
	.long	\e - kSignalNames
	.long	.L\@ - kSignalNames
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata
	.balign	4
	.underrun
kSignalNames:
	.e	SIGHUP,"SIGHUP"
	.e	SIGINT,"SIGINT"
	.e	SIGQUIT,"SIGQUIT"
	.e	SIGILL,"SIGILL"
	.e	SIGTRAP,"SIGTRAP"
	.e	SIGABRT,"SIGABRT"
	.e	SIGBUS,"SIGBUS"
	.e	SIGFPE,"SIGFPE"
	.e	SIGKILL,"SIGKILL"
	.e	SIGUSR1,"SIGUSR1"
	.e	SIGSEGV,"SIGSEGV"
	.e	SIGUSR2,"SIGUSR2"
	.e	SIGPIPE,"SIGPIPE"
	.e	SIGALRM,"SIGALRM"
	.e	SIGTERM,"SIGTERM"
	.e	SIGSTKFLT,"SIGSTKFLT"
	.e	SIGCHLD,"SIGCHLD"
	.e	SIGCONT,"SIGCONT"
	.e	SIGSTOP,"SIGSTOP"
	.e	SIGTSTP,"SIGTSTP"
	.e	SIGTTIN,"SIGTTIN"
	.e	SIGTTOU,"SIGTTOU"
	.e	SIGURG,"SIGURG"
	.e	SIGXCPU,"SIGXCPU"
	.e	SIGXFSZ,"SIGXFSZ"
	.e	SIGVTALRM,"SIGVTALRM"
	.e	SIGPROF,"SIGPROF"
	.e	SIGWINCH,"SIGWINCH"
	.e	SIGIO,"SIGIO"
	.e	SIGSYS,"SIGSYS"
	.e	SIGPWR,"SIGPWR"
	.e	SIGINFO,"SIGINFO"		// order matters
	.e	SIGTHR,"SIGTHR"			// order matters
	.e	SIGRTMAX,"SIGRTMAX"
	.e	SIGRTMIN,"SIGRTMIN"
	.e	SIGEMT,"SIGEMT"			// order matters
	.long	MAGNUM_TERMINATOR
	.endobj	kSignalNames,globl,hidden
	.overrun
