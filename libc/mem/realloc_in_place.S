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
#include "libc/notice.inc"
.source	__FILE__

/	Resizes the space allocated for p to size n, only if this can be
/	done without moving p (i.e., only if there is adjacent space
/	available if n is greater than p's current allocated size, or n
/	is less than or equal to p's size). This may be used instead of
/	plain realloc if an alternative allocation strategy is needed
/	upon failure to expand space, for example, reallocation of a
/	buffer that must be memory-aligned or cleared. You can use
/	realloc_in_place to trigger these alternatives only when needed.
/
/	@param	rdi (p) is address of current allocation
/	@param	rsi (newsize) is number of bytes needed
/	@return	rax is result, or NULL w/ errno
/	@see	dlrealloc_in_place()
realloc_in_place:
	jmp	*hook$realloc_in_place(%rip)
	.endfn	realloc_in_place,globl
