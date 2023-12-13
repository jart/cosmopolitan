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
	.long	\e - kClockNames
	.long	.L\@ - kClockNames
	.rodata.str1.1
.L\@:	.string	"\s"
	.previous
	.endm

	.section .rodata,"a",@progbits
	.balign	4
	.underrun
kClockNames:
	.e	CLOCK_REALTIME,"REALTIME"
	.e	CLOCK_REALTIME_FAST,"REALTIME_FAST"		// order matters
	.e	CLOCK_REALTIME_PRECISE,"REALTIME_PRECISE"	// order matters
	.e	CLOCK_REALTIME_COARSE,"REALTIME_COARSE"		// order matters
	.e	CLOCK_MONOTONIC,"MONOTONIC"
	.e	CLOCK_MONOTONIC_FAST,"MONOTONIC_FAST"		// order matters
	.e	CLOCK_MONOTONIC_RAW,"MONOTONIC_RAW"		// order matters
	.e	CLOCK_MONOTONIC_PRECISE,"MONOTONIC_PRECISE"	// order matters
	.e	CLOCK_MONOTONIC_COARSE,"MONOTONIC_COARSE"	// order matters
	.e	CLOCK_PROCESS_CPUTIME_ID,"PROCESS_CPUTIME_ID"
	.e	CLOCK_THREAD_CPUTIME_ID,"THREAD_CPUTIME_ID"
	.e	CLOCK_TAI,"TAI"
	.e	CLOCK_PROF,"PROF"
	.e	CLOCK_BOOTTIME,"BOOTTIME"
	.e	CLOCK_REALTIME_ALARM,"REALTIME_ALARM"
	.e	CLOCK_BOOTTIME_ALARM,"BOOTTIME_ALARM"
	.e	CLOCK_UPTIME,"UPTIME"
	.e	CLOCK_UPTIME_FAST,"UPTIME_FAST"
	.e	CLOCK_UPTIME_PRECISE,"UPTIME_PRECISE"
	.e	CLOCK_SECOND,"SECOND"
	.long	MAGNUM_TERMINATOR
	.endobj	kClockNames,globl,hidden
	.overrun
