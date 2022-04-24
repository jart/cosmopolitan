/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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
#include "libc/macros.internal.h"

	.macro	.e e
	.long	\e - kSockOptnames
	.long	1f - kSockOptnames
	.rodata.str1.1
1:	.string	"\e"
	.previous
	.endm

	.section .rodata
	.align 4
	.underrun
kSockOptnames:
	.e	SO_DEBUG			# bool32
	.e	SO_BROADCAST			# bool32
	.e	SO_REUSEADDR			# bool32
	.e	SO_REUSEPORT			# bool32
	.e	SO_KEEPALIVE			# bool32
	.e	SO_DONTROUTE			# bool32
	.e	SO_RCVTIMEO			# timeval
	.e	SO_SNDTIMEO			# timeval
	.e	SO_LINGER			# linger
	.e	SO_SNDBUF			# int
	.e	SO_RCVBUF			# int
	.e	SO_RCVLOWAT			# int
	.e	SO_SNDLOWAT			# int
	.e	SO_ERROR			# int
	.long	-123
	.endobj	kSockOptnames,globl,hidden
	.overrun
