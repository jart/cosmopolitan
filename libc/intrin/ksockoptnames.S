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
	.long	\e - kSockOptnames
	.long	.L\@ - kSockOptnames
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata
	.balign	4
	.underrun
kSockOptnames:
	.e	SO_DEBUG,"DEBUG"			// bool32
	.e	SO_ACCEPTCONN,"ACCEPTCONN"		// bool32
	.e	SO_BROADCAST,"BROADCAST"		// bool32
	.e	SO_REUSEADDR,"REUSEADDR"		// bool32
	.e	SO_REUSEPORT,"REUSEPORT"		// bool32
	.e	SO_KEEPALIVE,"KEEPALIVE"		// bool32
	.e	SO_DONTROUTE,"DONTROUTE"		// bool32
	.e	SO_RCVTIMEO,"RCVTIMEO"			// timeval
	.e	SO_SNDTIMEO,"SNDTIMEO"			// timeval
	.e	SO_LINGER,"LINGER"			// linger
	.e	SO_TYPE,"TYPE"				// int
	.e	SO_SNDBUF,"SNDBUF"			// int
	.e	SO_RCVBUF,"RCVBUF"			// int
	.e	SO_RCVLOWAT,"RCVLOWAT"			// int
	.e	SO_SNDLOWAT,"SNDLOWAT"			// int
	.e	SO_ERROR,"ERROR"			// int
	.long	MAGNUM_TERMINATOR
	.endobj	kSockOptnames,globl,hidden
	.overrun
