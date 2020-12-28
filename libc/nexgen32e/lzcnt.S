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

/	Finds leading bits in 𝑥.
/
/	      uint32 𝑥   bsf(𝑥) tzcnt(𝑥)   ffs(𝑥)   bsr(𝑥) lzcnt(𝑥)
/	    0x00000000      wut       32        0      wut       32
/	    0x00000001        0        0        1        0       31
/	    0x80000001        0        0        1       31        0
/	    0x80000000       31       31       32       31        0
/	    0x00000010        4        4        5        4       27
/	    0x08000010        4        4        5       27        4
/	    0x08000000       27       27       28       27        4
/	    0xffffffff        0        0        1       31        0
/
/	@param	edi is 32-bit unsigned 𝑥 value
/	@return	eax number in range [0,32) or 32 if 𝑥 is 0
/	@see	also treasure trove of nearly identical functions
lzcnt:	.leafprologue
	.profilable
	mov	$31,%eax
	mov	$-1,%edx
	bsr	%edi,%ecx
	cmovz	%edx,%ecx
	sub	%ecx,%eax
	.leafepilogue
	.endfn	lzcnt,globl
	.source	__FILE__
