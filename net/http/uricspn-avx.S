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

/	Verifies buffer contains only URI characters.
/
/	@param	%rdi is data which should be 32-byte aligned
/	@param	%rsi is byte length of data
/	@return	number of kosher bytes
/	@cost	10x faster than fastest Ragel code
uricspn$avx:
	.leafprologue
	.profilable
	vmovaps	.Luric(%rip),%xmm0
	mov	$14,%eax
	mov	%rsi,%rdx
	xor	%esi,%esi
0:	vmovdqu	(%rdi,%rsi),%xmm1
	vmovdqu	16(%rdi,%rsi),%xmm2
	vpcmpestri $0b00010100,%xmm1,%xmm0
	jc	1f
	jo	1f
	add	$16,%rsi
	sub	$16,%rdx
	vpcmpestri $0b00010100,%xmm2,%xmm0
	jc	1f
	jo	1f
	add	$16,%rsi
	sub	$16,%rdx
	jmp	0b
1:	lea	(%rsi,%rcx),%rax
	.leafepilogue
	.endfn	uricspn$avx,globl,hidden

	.rodata.cst16
.Luric:	.byte	'!,'!
	.byte	'$,';
	.byte	'=,'=
	.byte	'?,'Z
	.byte	'_,'_
	.byte	'a,'z
	.byte	'~,'~
	.byte	0,0
	.endobj	.Luric
	.previous
