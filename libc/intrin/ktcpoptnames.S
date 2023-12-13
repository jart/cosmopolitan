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
	.long	\e - kTcpOptnames
	.long	.L\@ - kTcpOptnames
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata
	.balign	4
	.underrun
kTcpOptnames:
	.e	TCP_NODELAY,"NODELAY"			// bool32
	.e	TCP_CORK,"CORK"				// bool32
	.e	TCP_QUICKACK,"QUICKACK"			// bool32
	.e	TCP_FASTOPEN_CONNECT,"FASTOPEN_CONNECT"	// bool32
	.e	TCP_DEFER_ACCEPT,"DEFER_ACCEPT"		// bool32
	.e	TCP_KEEPIDLE,"KEEPIDLE"			// int (seconds)
	.e	TCP_KEEPINTVL,"KEEPINTVL"		// int (seconds)
	.e	TCP_FASTOPEN,"FASTOPEN"			// int
	.e	TCP_KEEPCNT,"KEEPCNT"			// int
	.e	TCP_MAXSEG,"MAXSEG"			// int
	.e	TCP_SYNCNT,"SYNCNT"			// int
	.e	TCP_NOTSENT_LOWAT,"NOTSENT_LOWAT"	// int
	.e	TCP_WINDOW_CLAMP,"WINDOW_CLAMP"		// int
	.e	TCP_SAVE_SYN,"SAVE_SYN"			// int
	.e	TCP_SAVED_SYN,"SAVED_SYN"		// buffer
	.long	MAGNUM_TERMINATOR
	.endobj	kTcpOptnames,globl,hidden
	.overrun
