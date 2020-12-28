/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/macros.h"

/	Support code for fast integer division by Si units.
/
/	Division by magnums is described in Hacker's Delight and is
/	usually generated automatically by compilers, but sadly not
/	when we optimize for size and idiv goes at least 10x slower
/	so we do this which saves space while avoiding build tuning
/
/	@param	rdi is number to divide
/	@param	cl is magnum #1
/	@param	rdx is magnum #2
/	@return	quotient
tinydivsi:
	.leafprologue
	mov	%rdi,%rax
	imul	%rdx
	mov	%rdx,%rax
	sar	%cl,%rax
	sar	$63,%rdi
	sub	%rdi,%rax
	.leafepilogue
	.endfn	tinydivsi,globl
	.source	__FILE__
