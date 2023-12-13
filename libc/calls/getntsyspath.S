/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
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
#include "libc/dce.h"
#include "libc/macros.internal.h"

//	Obtains WIN32 magic path, e.g. GetTempPathA.
//
//	@param	rax is address of ANSI path provider function
//	@param	rdi is output buffer
//	@param	rdx is output buffer size in bytes that's >0
//	@return	eax is string length w/ NUL that's ≤ edx
//	@return	rdi is rdi+edx
	.text.startup
__getntsyspath:
	push	%rbp
	mov	%rsp,%rbp
	push	%rdx
	movpp	%rdi,%rcx		# call f=%rax(p1=%rcx,p2=%rdx)
	sub	$40,%rsp
	call	*%rax
	testb	IsWindows()
	jz	3f
	mov	(%rdi),%cl		# turn c:\... into \c\...
	movb	$'\\',(%rdi)
	mov	%cl,1(%rdi)
	movb	$'\\',2(%rdi)
3:	xor	%edx,%edx
	mov	-8(%rbp),%ecx		# restore %edx param as %ecx
	cmp	%eax,%ecx		# use current dir on overflow
	cmovbe	%edx,%eax
	cmp	$1,%eax			# leave empty strings empty
	jbe	1f
	cmpb	$'\\',-1(%rdi,%rax)	# guarantee trailing slash
	je	1f
	movw	$'\\',(%rdi,%rax)
	inc	%eax
1:	inc	%rdi			# change backslash to slash
	cmpb	$'\\',-1(%rdi)
	jne	2f
	movb	$'/',-1(%rdi)
2:	.loop	1b
	leave
	ret
	.endfn	__getntsyspath,globl,hidden
