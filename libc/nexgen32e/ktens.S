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

//	static const uint64_t kTens[] = {
//	    1ull,
//	    10ull,
//	    100ull,
//	    1000ull,
//	    10000ull,
//	    100000ull,
//	    1000000ull,
//	    10000000ull,
//	    100000000ull,
//	    1000000000ull,
//	    10000000000ull,
//	    100000000000ull,
//	    1000000000000ull,
//	    10000000000000ull,
//	    100000000000000ull,
//	    1000000000000000ull,
//	    10000000000000000ull,
//	    100000000000000000ull,
//	    1000000000000000000ull,
//	    10000000000000000000ull,
//	};

	.initbss 201,_init_kTens
kTens:	.rept	20
	.quad	0
	.endr
	.endobj	kTens,globl
	.previous

	.init.start 201,_init_kTens
	push	$20
	pop	%rcx
	push	$10
	pop	%r8
	push	$1
	pop	%rax
1:	stosq
	mul	%r8
	.loop	1b
	.init.end 201,_init_kTens
