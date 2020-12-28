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
.source	__FILE__

/	Encodes Thompson-Pike varint.
/
/	@param	edi is int to encode
/	@return	rax is word-encoded byte buffer
/	@note	invented on a napkin in a new jersey diner
tpenc:	.leafprologue
	.profilable
	mov	%edi,%edi
	xor	%eax,%eax
	cmp	$127,%edi
	jbe	3f
	bsr	%edi,%ecx
	mov	kTpenc-7*(1+1)(,%rcx,2),%ecx
1:	mov	%edi,%edx
	shr	$6,%edi
	and	$0b00111111,%dl
	or	$0b10000000,%al
	or	%dl,%al
	shl	$8,%rax
	dec	%cl
	jnz	1b
2:	or	%ch,%al
3:	or	%rdi,%rax
	.leafepilogue
	.endfn	tpenc,globl

	.rodata
	.align	4
kTpenc:	.rept	4			# MSB≤10 (0x7FF)
	.byte	1,0b11000000		# len,mark
	.endr
	.rept	5			# MSB≤15 (0xFFFF)
	.byte	2,0b11100000		# len,mark
	.endr
	.rept	5			# MSB≤20 (0x1FFFFF)
	.byte	3,0b11110000		# len,mark
	.endr
	.rept	5			# MSB≤25 (0x3FFFFFF)
	.byte	4,0b11111000		# len,mark
	.endr
	.rept	6			# MSB≤31 (0xffffffff)
	.byte	5,0b11111100		# len,mark
	.endr
	.zero	2
	.endobj	kTpenc
