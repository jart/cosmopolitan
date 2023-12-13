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
#include "libc/calls/struct/timespec.h"
#include "libc/macros.internal.h"

nsync_time_now:
	jmp	timespec_real
	.endfn	nsync_time_now,globl

nsync_time_add:
	jmp	timespec_add
	.endfn	nsync_time_add,globl

nsync_time_sub:
	jmp	timespec_sub
	.endfn	nsync_time_sub,globl

nsync_time_cmp:
	jmp	timespec_cmp
	.endfn	nsync_time_cmp,globl

nsync_time_ms:
	jmp	timespec_frommillis
	.endfn	nsync_time_ms,globl

nsync_time_us:
	jmp	timespec_frommicros
	.endfn	nsync_time_us,globl

nsync_time_sleep:
	jmp	timespec_sleep
	.endfn	nsync_time_us,globl

nsync_time_sleep_until:
	jmp	timespec_sleep_until
	.endfn	nsync_time_us,globl
