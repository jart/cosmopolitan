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

/	Returns prefix length, consisting of chars not in reject.
/	a.k.a. Return index of first byte that's in charset.
/
/	@param	rdi is string
/	@param	rsi is reject nul-terminated character set
/	@see	strspn(), strtok_r()
/	@asyncsignalsafe
strcspn:
	push	%rbp
	mov	%rsp,%rbp
	sub	$16,%rsp
	push	%rdi
	mov	%rsi,%rdi
	call	strlen
	pop	%rdi
	cmp	$15,%rax
	ja	4f
	push	%rdi
	mov	%rax,%rdx
	pxor	%xmm0,%xmm0
	lea	-16(%rbp),%rdi
	movdqa	%xmm0,(%rdi)
	call	MemCpy
	movdqa	(%rdi),%xmm1
	pop	%rdi
	or	$-1,%rax
0:	inc	%rax
	movzbl	(%rdi,%rax),%ecx
	movd	%ecx,%xmm0
	punpcklbw %xmm0,%xmm0
	punpcklwd %xmm0,%xmm0
	pshufd	$0,%xmm0,%xmm0
	pcmpeqb	%xmm1,%xmm0
	pmovmskb %xmm0,%ecx
	test	%ecx,%ecx
	jz	0b
9:	leave
	ret
1:	cmp	%ch,%cl
	je	9b
	inc	%edx
2:	mov	(%rsi,%rdx),%ch
	test	%ch,%ch
	jne	1b
	inc	%rax
3:	mov	(%rdi,%rax),%cl
	test	%cl,%cl
	je	9b
	xor	%edx,%edx
	jmp	2b
4:	xor	%eax,%eax
	jmp	3b
	.endfn	strcspn,globl
