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

/	Compares strings w/ limit & no-clobber greg abi.
/
/	@param	%rdi is first string
/	@param	%rsi is second string
/	@param	%rdx is max length
/	@return	<0, 0, or >0 depending on comparison
/	@clob	flags only
/	@asyncsignalsafe
tinystrncmp:
	.leafprologue
	push	%rbx
	push	%rcx
	xor	%eax,%eax
	xor	%ebx,%ebx
	xor	%ecx,%ecx
	test	%edx,%edx
	jz	2f
	cmp	%rdi,%rsi
	je	2f
0:	cmp	%edx,%ecx
	jae	1f
	movzbl	(%rdi,%rcx,1),%eax
	movzbl	(%rsi,%rcx,1),%ebx
	test	%al,%al
	jz	1f
	cmp	%bl,%al
	jne	1f
	inc	%ecx
	jmp	0b
1:	sub	%ebx,%eax
2:	pop	%rcx
	pop	%rbx
	.leafepilogue
	.endfn	tinystrncmp,globl
	.source	__FILE__
