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

/	Copies memory.
/
/	DEST and SRC may overlap.
/
/	@param	rdi is dest
/	@param	rsi is src
/	@param	rdx is number of bytes
/	@return	original rdi copied to rax
/	@clob	flags,rcx
/	@asyncsignalsafe
memmove:
	mov	%rdi,%rax
/	𝑠𝑙𝑖𝑑𝑒
	.endfn	MemMove,globl,hidden

MemMove:
	.leafprologue
	.profilable
	push	%rdi
	push	%rsi
	mov	%rdx,%rcx
	cmp	%rsi,%rdi
	jb	1f
	lea	-1(%rdi,%rcx),%rdi
	lea	-1(%rsi,%rcx),%rsi
	std
1:	rep movsb
	cld
	pop	%rsi
	pop	%rdi
	.leafepilogue
	.endfn	memmove,globl
	.source	__FILE__
