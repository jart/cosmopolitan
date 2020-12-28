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
stbi__YCbCr_to_RGB_row$sse2:
	.leafprologue
	.profilable
	xor	%eax,%eax
	cmp	$8,%r8d
	jl	1f
	xor	%eax,%eax
	movdqa	2f(%rip),%xmm2
	movdqa	3f(%rip),%xmm8
	movdqa	4f(%rip),%xmm9
	movdqa	5f(%rip),%xmm10
	movdqa	6f(%rip),%xmm4
	movdqa	7f(%rip),%xmm5
	.align	16
0:	movq	(%rsi,%rax),%xmm6
	movq	(%rcx,%rax),%xmm7
	movq	(%rdx,%rax),%xmm1
	movdqa	%xmm2,%xmm0
	punpcklbw %xmm6,%xmm0
	pxor	%xmm2,%xmm7
	pxor	%xmm6,%xmm6
	punpcklbw %xmm7,%xmm6
	pxor	%xmm2,%xmm1
	pxor	%xmm3,%xmm3
	punpcklbw %xmm1,%xmm3
	psrlw	$4,%xmm0
	movdqa	%xmm6,%xmm7
	pmulhw	%xmm8,%xmm7
	movdqa	%xmm3,%xmm1
	pmulhw	%xmm9,%xmm1
	pmulhw	%xmm10,%xmm3
	pmulhw	%xmm4,%xmm6
	paddw	%xmm1,%xmm6
	paddw	%xmm0,%xmm7
	paddw	%xmm0,%xmm3
	paddw	%xmm0,%xmm6
	psraw	$4,%xmm7
	psraw	$4,%xmm3
	packuswb %xmm3,%xmm7
	psraw	$4,%xmm6
	packuswb %xmm5,%xmm6
	movdqa	%xmm7,%xmm0
	punpcklbw %xmm6,%xmm0
	punpckhbw %xmm6,%xmm7
	movdqa	%xmm0,%xmm1
	punpcklwd %xmm7,%xmm1
	punpckhwd %xmm7,%xmm0
	movdqu	%xmm1,(%rdi,%rax,4)
	movdqu	%xmm0,16(%rdi,%rax,4)
	add	$8,%rax
	lea	7(%rax),%r9d
	cmp	%r8d,%r9d
	jl	0b
1:	.leafepilogue
	.endfn	stbi__YCbCr_to_RGB_row$sse2,globl

	.rodata.cst16
2:	.byte	128,128,128,128,128,128,128,128
	.zero	8
3:	.short	5743,5743,5743,5743,5743,5743,5743,5743
4:	.short	64126,64126,64126,64126,64126,64126,64126,64126
5:	.short	7258,7258,7258,7258,7258,7258,7258,7258
6:	.short	62611,62611,62611,62611,62611,62611,62611,62611
7:	.short	255,255,255,255,255,255,255,255

	.end
/	These should be better but need to get them to work
3:	.short	11485,11485,11485,11485,11485,11485,11485,11485		# J′R m=13 99.964387%
4:	.short	-11277,-11277,-11277,-11277,-11277,-11277,-11277,-11277	# J′G m=15 99.935941%
5:	.short	14516,14516,14516,14516,14516,14516,14516,14516		# J′B m=13 99.947219%
6:	.short	-23401,-23401,-23401,-23401,-23401,-23401,-23401,-23401	# J′G m=15 99.935941%
7:	.short	255,255,255,255,255,255,255,255
