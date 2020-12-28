/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 sw=8 fenc=utf-8                                     :vi│
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

/	Fast log₁₀ when 𝑥 is an integer.
/
/	@param	rdi is uint64 𝑥
/	@domain	0<𝑥<2⁶⁴ ∧ 𝑥∊ℤ
llog10:	.leafprologue
	.profilable
	bsr	%rdi,%rax
	jz	3f
	lea	llog10data(%rip),%rdx
	movsbl	1(%rdx,%rax),%eax
	cmp	2f-1f(%rdx,%rax,8),%rdi
	sbb	$0,%al
	jmp	4f
3:	xor	%eax,%eax	# domain error
4:	.leafepilogue
	.endfn	llog10,globl

	.rodata
llog10data:
1:	.byte	 0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4
	.byte	 4, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9
	.byte	 9, 9,10,10,10,11,11,11,12,12,12,12,13,13,13,14
	.byte	14,14,15,15,15,15,16,16,16,17,17,17,18,18,18,18
	.byte	19,19,19
	.align	8
2:	.quad	0
	.quad	10
	.quad	100
	.quad	1000
	.quad	10000
	.quad	100000
	.quad	1000000
	.quad	10000000
	.quad	100000000
	.quad	1000000000
	.quad	10000000000
	.quad	100000000000
	.quad	1000000000000
	.quad	10000000000000
	.quad	100000000000000
	.quad	1000000000000000
	.quad	10000000000000000
	.quad	100000000000000000
	.endobj	llog10data
	.previous
	.source	__FILE__
