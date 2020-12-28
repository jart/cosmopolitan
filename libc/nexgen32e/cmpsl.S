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

/	Compares 32-bit signed integers.
/
/	@param	rdi points to left integer
/	@param	rsi points to right integer
/	@return	<0, 0, or >0 based on comparison
	.align	16
cmpsl:	.leafprologue
	.profilable
	xor	%eax,%eax
	cmpsl
/	mov	(%rdi),%edi
/	mov	(%rsi),%esi
/	cmp	%edi,%esi
	setl	%al
	cmovg	.Lneg1(%rip),%eax
	.leafepilogue
	.endfn	cmpsl,globl

	.rodata.cst4
.Lone:	.long	1
	.endobj	.Lone
.Lneg1:	.long	-1
	.endobj	.Lneg1
	.previous
	.source	__FILE__
