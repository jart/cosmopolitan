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

/	Creates bit mask of which bytes are the same.
/
/	@param	%rdi points to bit mask (write-only)
/	@param	%rsi points to first buffer (read-only)
/	@param	%rdx points to second buffer (read-only)
/	@param	%rcx is byte length of both %rsi and %rdx
/	@return	%rax is set to %rdi
/	@note	buffers should be 128-byte aligned
memeqmask:
	.leafprologue
	xor	%eax,%eax
	test	%ecx,%ecx
	jz	1f
	shr	$3,%ecx
0:	movdqa	(%rsi,%rax,8),%xmm0
	movdqa	16(%rsi,%rax,8),%xmm1
	pcmpeqb	(%rdx,%rax,8),%xmm0
	pcmpeqb	16(%rdx,%rax,8),%xmm1
	vpmovmskb %xmm0,%r8d
	vpmovmskb %xmm1,%r9d
	mov	%r8w,(%rdi,%rax)
	mov	%r9w,2(%rdi,%rax)
	add	$4,%eax
	cmp	%ecx,%eax
	jb	0b
1:	mov	%rdi,%rax
	.leafepilogue
	.endfn	memeqmask,globl
	.source	__FILE__
