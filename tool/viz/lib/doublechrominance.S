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
#include "libc/macros.internal.h"

//	Duplicates chrominance samples horizontally, e.g.
//
//	    12345678--------
//	  → 1122334455667788
//
//	@param	%edi is size of %rsi array in bytes
//	@param	%rsi is char[edi/16][16] output and %rsi==%rdx is OK
//	@param	%rdx is char[edi/16][8] input
//	@return	%rax is %rsi
	.ftrace1
doublechrominance:
	.ftrace2
	.leafprologue
	shr	$1,%edi			# backwards algorithm
	jbe	1f			# do nothing if !n || n%2
	mov	%edi,%ecx
	shr	$4,%ecx
	shl	$4,%ecx
0:	cmp	%edi,%ecx
	je	0f
	dec	%edi
	movzbl	(%rdx,%rdi),%eax
	mov	%al,(%rsi,%rdi,2)
	mov	%al,1(%rsi,%rdi,2)
	jmp	0b
0:	sub	$8,%edi
	movq	(%rdx,%rdi),%xmm0
	punpcklbw %xmm0,%xmm0
	movdqu	%xmm0,(%rsi,%rdi,2)
	jnz	0b
1:	mov	%rsi,%rax
	.leafepilogue
	.endfn	doublechrominance,globl
