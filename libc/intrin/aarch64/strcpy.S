/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│ vi: set noet ft=asm ts=8 sw=8 fenc=utf-8                                 :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Optimized Routines                                                          │
│  Copyright (c) 1999-2022, Arm Limited.                                       │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files (the             │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and/or sell copies of the Software, and to          │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│  The above copyright notice and this permission notice shall be              │
│  included in all copies or substantial portions of the Software.             │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.      │
│  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY        │
│  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,        │
│  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE           │
│  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                      │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/intrin/aarch64/asmdefs.internal.h"

#define __strcpy_aarch64 strcpy

.ident "\n\n\
Optimized Routines (MIT License)\n\
Copyright 2022 ARM Limited\n"
.include "libc/disclaimer.inc"

/* Assumptions:
 *
 * ARMv8-a, AArch64, Advanced SIMD.
 * MTE compatible.
 */

#define dstin		x0
#define srcin		x1
#define result		x0

#define src		x2
#define dst		x3
#define len		x4
#define synd		x4
#define	tmp		x5
#define shift		x5
#define data1		x6
#define dataw1		w6
#define data2		x7
#define dataw2		w7

#define dataq		q0
#define vdata		v0
#define vhas_nul	v1
#define vend		v2
#define dend		d2
#define dataq2		q1

/*
   Core algorithm:
   For each 16-byte chunk we calculate a 64-bit nibble mask value with four bits
   per byte. We take 4 bits of every comparison byte with shift right and narrow
   by 4 instruction. Since the bits in the nibble mask reflect the order in
   which things occur in the original string, counting leading zeros identifies
   exactly which byte matched.  */

ENTRY (__strcpy_aarch64)
	PTR_ARG (0)
	PTR_ARG (1)
	bic	src, srcin, 15
	ld1	{vdata.16b}, [src]
	cmeq	vhas_nul.16b, vdata.16b, 0
	lsl	shift, srcin, 2
	shrn	vend.8b, vhas_nul.8h, 4
	fmov	synd, dend
	lsr	synd, synd, shift
	cbnz	synd, L(tail)

	ldr	dataq, [src, 16]!
	cmeq	vhas_nul.16b, vdata.16b, 0
	shrn	vend.8b, vhas_nul.8h, 4
	fmov	synd, dend
	cbz	synd, L(start_loop)

#ifndef __AARCH64EB__
	rbit	synd, synd
#endif
	sub	tmp, src, srcin
	clz	len, synd
	add	len, tmp, len, lsr 2
	tbz	len, 4, L(less16)
	sub	tmp, len, 15
	ldr	dataq, [srcin]
	ldr	dataq2, [srcin, tmp]
	str	dataq, [dstin]
	str	dataq2, [dstin, tmp]
	ret

L(tail):
	rbit	synd, synd
	clz	len, synd
	lsr	len, len, 2
L(less16):
	tbz	len, 3, L(less8)
	sub	tmp, len, 7
	ldr	data1, [srcin]
	ldr	data2, [srcin, tmp]
	str	data1, [dstin]
	str	data2, [dstin, tmp]
	ret

	.p2align 4
L(less8):
	subs	tmp, len, 3
	b.lo	L(less4)
	ldr	dataw1, [srcin]
	ldr	dataw2, [srcin, tmp]
	str	dataw1, [dstin]
	str	dataw2, [dstin, tmp]
	ret

L(less4):
	cbz	len, L(zerobyte)
	ldrh	dataw1, [srcin]
	strh	dataw1, [dstin]
L(zerobyte):
	strb	wzr, [dstin, len]
	ret

	.p2align 4
L(start_loop):
	sub	tmp, srcin, dstin
	ldr	dataq2, [srcin]
	sub	dst, src, tmp
	str	dataq2, [dstin]
L(loop):
	str	dataq, [dst], 32
	ldr	dataq, [src, 16]
	cmeq	vhas_nul.16b, vdata.16b, 0
	umaxp	vend.16b, vhas_nul.16b, vhas_nul.16b
	fmov	synd, dend
	cbnz	synd, L(loopend)
	str	dataq, [dst, -16]
	ldr	dataq, [src, 32]!
	cmeq	vhas_nul.16b, vdata.16b, 0
	umaxp	vend.16b, vhas_nul.16b, vhas_nul.16b
	fmov	synd, dend
	cbz	synd, L(loop)
	add	dst, dst, 16
L(loopend):
	shrn	vend.8b, vhas_nul.8h, 4		/* 128->64 */
	fmov	synd, dend
	sub	dst, dst, 31
#ifndef __AARCH64EB__
	rbit	synd, synd
#endif
	clz	len, synd
	lsr	len, len, 2
	add	dst, dst, len
	ldr	dataq, [dst, tmp]
	str	dataq, [dst]
	ret

END (__strcpy_aarch64)
