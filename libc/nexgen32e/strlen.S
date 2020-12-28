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

/	Returns length of NUL-terminated string.
/
/	@param	rdi is non-null NUL-terminated string pointer
/	@return	rax is number of bytes (excluding NUL)
/	@clob	ax,dx,cx,xmm3,xmm4
/	@note	h/t agner fog
/	@asyncsignalsafe
strlen:	.leafprologue
	.profilable
	mov	%rdi,%rax
	mov	%edi,%ecx
	and	$15,%ecx
	and	$-16,%rax
	pxor	%xmm4,%xmm4
	movdqa	(%rax),%xmm3
	pcmpeqb	%xmm4,%xmm3
	pmovmskb %xmm3,%edx
	shr	%cl,%edx
	shl	%cl,%edx
	bsf	%edx,%edx
	jnz	2f
1:	lea	16(%rax),%rax
	movdqa	(%rax),%xmm3
	pcmpeqb	%xmm4,%xmm3
	pmovmskb %xmm3,%edx
	bsf	%edx,%edx
	jz	1b
2:	add	%rdx,%rax
	sub	%rdi,%rax
	.leafepilogue
	.endfn	strlen,globl
