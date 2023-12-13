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
	.long	\e - kRlimitNames
	.long	.L\@ - kRlimitNames
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata
	.balign	4
	.underrun
kRlimitNames:
	.e	RLIMIT_AS,"AS"
	.e	RLIMIT_CPU,"CPU"
	.e	RLIMIT_FSIZE,"FSIZE"
	.e	RLIMIT_NPROC,"NPROC"
	.e	RLIMIT_NOFILE,"NOFILE"
	.e	RLIMIT_RSS,"RSS"
	.e	RLIMIT_DATA,"DATA"
	.e	RLIMIT_CORE,"CORE"
	.e	RLIMIT_STACK,"STACK"
	.e	RLIMIT_SIGPENDING,"SIGPENDING"
	.e	RLIMIT_MEMLOCK,"MEMLOCK"
	.e	RLIMIT_LOCKS,"LOCKS"
	.e	RLIMIT_MSGQUEUE,"MSGQUEUE"
	.e	RLIMIT_NICE,"NICE"
	.e	RLIMIT_RTPRIO,"RTPRIO"
	.e	RLIMIT_RTTIME,"RTTIME"
	.e	RLIMIT_SWAP,"SWAP"
	.e	RLIMIT_SBSIZE,"SBSIZE"
	.e	RLIMIT_NPTS,"NPTS"
	.long	MAGNUM_TERMINATOR
	.endobj	kRlimitNames,globl,hidden
	.overrun
