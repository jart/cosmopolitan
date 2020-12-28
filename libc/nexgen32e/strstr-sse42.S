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
#include "libc/nexgen32e/pcmpstr.inc"
#include "libc/nexgen32e/strstr.inc"

/	TODO(jart): Fix me.
strstr$sse42:
	.leafprologue
	mov	%rdi,%rax
	xor	%ecx,%ecx
0:	mov	$-16,%rdx
1:	add	$16,%rdx
	movaps	(%rsi,%rdx),%xmm0
2:	add	%rcx,%rax
	lea	(%rax,%rdx),%rdi
	pcmpistri $.Lequalordered,(%rdi),%xmm0
3:	ja	2b	# !CF (no match) && !ZF (need NUL-term)
	jnc	4f	# !CF (no match) && ZF (NUL-terminator)
	jno	0b	# !OF ← CF && CX!=0 (matched at offset)
	jns	1b	# !SF ← NUL ∉ XMM1 (need to match more)
	jmp	5f	# youtu.be/nVk1DjMtLWs
4:	xor	%eax,%eax
5:	.leafepilogue
	.endfn	strstr$sse42,globl,hidden
	.source	__FILE__
