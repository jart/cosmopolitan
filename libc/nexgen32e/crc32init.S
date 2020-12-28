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

/	Generates lookup table for computing CRC-32 byte-by-byte.
/
/	    void crc32init(uint32_t table[256], uint32_t polynomial) {
/	      uint32_t d, i, r;
/	      for (d = 0; d < 256; ++d) {
/	        r = d;
/	        for (i = 0; i < 8; ++i) {
/	          r = r >> 1 ^ (r & 1 ? polynomial : 0);
/	        }
/	        table[d] = r;
/	      }
/	    }
/
/	@param	rdi is pointer to uint32_t[256] array
/	@param	esi 32-bit binary polynomial config
/	@note	imposes ~300ns one-time cost
crc32init:
	push	%rbp
	mov	%rsp,%rbp
	.profilable
	lea	256*4(%rdi),%rdx
	movd	%esi,%xmm0
	pshufd	$0,%xmm0,%xmm0		# (uint32_t[]){esi,esi,esi,esi} → %xmm0
	pushpop	4,%rax
	movd	%eax,%xmm2		# (int[]){4,4,4,4} → %xmm2
	pshufd	$0,%xmm2,%xmm2
0:	sub	$4,%rsp			# (int[]){0,1,2,3} → %xmm1
	dec	%eax
	mov	%eax,(%rsp)
	jnz	0b
	movdqu	(%rsp),%xmm1
1:	mov	$8,%ecx
	movdqa	%xmm1,%xmm3
2:	movdqa	%xmm3,%xmm4
	psrld	$1,%xmm4
	pslld	$31,%xmm3
	psrad	$31,%xmm3
	pand	%xmm0,%xmm3
	pxor	%xmm4,%xmm3
	movdqa	%xmm3,%xmm4
	.loop	2b
	movdqu	%xmm3,(%rdi)
	add	$16,%rdi
	paddd	%xmm2,%xmm1
	cmp	%rdx,%rdi
	jb	1b
	leave
	ret
	.endfn	crc32init,globl
	.source	__FILE__
