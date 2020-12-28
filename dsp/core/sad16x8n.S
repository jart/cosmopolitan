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
.align	16

/	Mixes audio.
/
/	@param	rdi is # aligned int16[16] sample chunks to process
/	@param	rsi points to aligned pcm s16le input/output memory
/	@param	rdx points to aligned pcm s16le [0..1] input memory
sad16x8n:
	.leafprologue
	.profilable
	test	%rdi,%rdi
	jz	1f
	shl	$3,%rdi
0:	sub	$8,%rdi
	movdqa	(%rsi,%rdi,2),%xmm0
	paddsw	(%rdx,%rdi,2),%xmm0
	movdqa	%xmm0,(%rsi,%rdi,2)
	jnz	0b
1:	.leafepilogue
	.endfn	sad16x8n,globl,hidden
	.source	__FILE__
