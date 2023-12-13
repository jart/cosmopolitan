/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
	.long	\e - kFcntlCmds
	.long	.L\@ - kFcntlCmds
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata,"a",@progbits
	.balign 4
	.underrun
kFcntlCmds:
	.e	F_GETFD,"GETFD"
	.e	F_SETFD,"SETFD"
	.e	F_GETFL,"GETFL"
	.e	F_SETFL,"SETFL"
	.e	F_DUPFD,"DUPFD"
	.e	F_DUPFD_CLOEXEC,"DUPFD_CLOEXEC"
	.e	F_GETLK,"GETLK"
	.e	F_SETLK,"SETLK"
	.e	F_SETLKW,"SETLKW"
	.e	F_GETOWN,"GETOWN"
	.e	F_SETOWN,"SETOWN"
	.e	F_GETPATH,"GETPATH"
	.e	F_NOCACHE,"NOCACHE"
	.e	F_FULLFSYNC,"FULLFSYNC"
	.e	F_OFD_GETLK,"OFD_GETLK"
	.e	F_OFD_SETLK,"OFD_SETLK"
	.e	F_OFD_SETLKW,"OFD_SETLKW"
	.e	F_BARRIERFSYNC,"BARRIERFSYNC"
	.e	F_SETNOSIGPIPE,"SETNOSIGPIPE"
	.e	F_GETNOSIGPIPE,"GETNOSIGPIPE"
	.e	F_MAXFD,"MAXFD"
	.long	MAGNUM_TERMINATOR
	.endobj	kFcntlCmds,globl,hidden
	.overrun
