/*-*- mode:unix-assembly; indent-tabs-mode:t; tab-width:8; coding:utf-8     -*-│
│vi: set et ft=asm ts=8 tw=8 fenc=utf-8                                     :vi│
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

#define __memcmp_aarch64 memcmp

.ident "\n\n\
Optimized Routines (MIT License)\n\
Copyright 2022 ARM Limited\n"
.include "libc/disclaimer.inc"

/* Assumptions:
 *
 * ARMv8-a, AArch64, Advanced SIMD, unaligned accesses.
 */

#define src1	x0
#define src2	x1
#define limit	x2
#define result	w0

#define data1	x3
#define data1w	w3
#define data2	x4
#define data2w	w4
#define data3	x5
#define data3w	w5
#define data4	x6
#define data4w	w6
#define tmp	x6
#define src1end	x7
#define src2end	x8


ENTRY (__memcmp_aarch64)
	PTR_ARG (0)
	PTR_ARG (1)
	SIZE_ARG (2)

	cmp	limit, 16
	b.lo	L(less16)
	ldp	data1, data3, [src1]
	ldp	data2, data4, [src2]
	ccmp	data1, data2, 0, ne
	ccmp	data3, data4, 0, eq
	b.ne	L(return2)

	add	src1end, src1, limit
	add	src2end, src2, limit
	cmp	limit, 32
	b.ls	L(last_bytes)
	cmp	limit, 160
	b.hs	L(loop_align)
	sub	limit, limit, 32

	.p2align 4
L(loop32):
	ldp	data1, data3, [src1, 16]
	ldp	data2, data4, [src2, 16]
	cmp	data1, data2
	ccmp	data3, data4, 0, eq
	b.ne	L(return2)
	cmp	limit, 16
	b.ls	L(last_bytes)

	ldp	data1, data3, [src1, 32]
	ldp	data2, data4, [src2, 32]
	cmp	data1, data2
	ccmp	data3, data4, 0, eq
	b.ne	L(return2)
	add	src1, src1, 32
	add	src2, src2, 32
L(last64):
	subs	limit, limit, 32
	b.hi	L(loop32)

	/* Compare last 1-16 bytes using unaligned access.  */
L(last_bytes):
	ldp	data1, data3, [src1end, -16]
	ldp	data2, data4, [src2end, -16]
L(return2):
	cmp	data1, data2
	csel	data1, data1, data3, ne
	csel	data2, data2, data4, ne

	/* Compare data bytes and set return value to 0, -1 or 1.  */
L(return):
#ifndef __AARCH64EB__
	rev	data1, data1
	rev	data2, data2
#endif
	cmp	data1, data2
	cset	result, ne
	cneg	result, result, lo
	ret

	.p2align 4
L(less16):
	add	src1end, src1, limit
	add	src2end, src2, limit
	tbz	limit, 3, L(less8)
	ldr	data1, [src1]
	ldr	data2, [src2]
	ldr	data3, [src1end, -8]
	ldr	data4, [src2end, -8]
	b	L(return2)

	.p2align 4
L(less8):
	tbz	limit, 2, L(less4)
	ldr	data1w, [src1]
	ldr	data2w, [src2]
	ldr	data3w, [src1end, -4]
	ldr	data4w, [src2end, -4]
	b	L(return2)

L(less4):
	tbz	limit, 1, L(less2)
	ldrh	data1w, [src1]
	ldrh	data2w, [src2]
	cmp	data1w, data2w
	b.ne	L(return)
L(less2):
	mov	result, 0
	tbz	limit, 0, L(return_zero)
	ldrb	data1w, [src1end, -1]
	ldrb	data2w, [src2end, -1]
	sub	result, data1w, data2w
L(return_zero):
	ret

L(loop_align):
	ldp	data1, data3, [src1, 16]
	ldp	data2, data4, [src2, 16]
	cmp	data1, data2
	ccmp	data3, data4, 0, eq
	b.ne	L(return2)

	/* Align src2 and adjust src1, src2 and limit.  */
	and	tmp, src2, 15
	sub	tmp, tmp, 16
	sub	src2, src2, tmp
	add	limit, limit, tmp
	sub	src1, src1, tmp
	sub	limit, limit, 64 + 16

	.p2align 4
L(loop64):
	ldr	q0, [src1, 16]
	ldr	q1, [src2, 16]
	subs	limit, limit, 64
	ldr	q2, [src1, 32]
	ldr	q3, [src2, 32]
	eor	v0.16b, v0.16b, v1.16b
	eor	v1.16b, v2.16b, v3.16b
	ldr	q2, [src1, 48]
	ldr	q3, [src2, 48]
	umaxp	v0.16b, v0.16b, v1.16b
	ldr	q4, [src1, 64]!
	ldr	q5, [src2, 64]!
	eor	v1.16b, v2.16b, v3.16b
	eor	v2.16b, v4.16b, v5.16b
	umaxp	v1.16b, v1.16b, v2.16b
	umaxp	v0.16b, v0.16b, v1.16b
	umaxp	v0.16b, v0.16b, v0.16b
	fmov	tmp, d0
	ccmp	tmp, 0, 0, hi
	b.eq	L(loop64)

	/* If equal, process last 1-64 bytes using scalar loop.  */
	add	limit, limit, 64 + 16
	cbz	tmp, L(last64)

	/* Determine the 8-byte aligned offset of the first difference.  */
#ifdef __AARCH64EB__
	rev16	tmp, tmp
#endif
	rev	tmp, tmp
	clz	tmp, tmp
	bic	tmp, tmp, 7
	sub	tmp, tmp, 48
	ldr	data1, [src1, tmp]
	ldr	data2, [src2, tmp]
#ifndef __AARCH64EB__
	rev	data1, data1
	rev	data2, data2
#endif
	mov	result, 1
	cmp	data1, data2
	cneg	result, result, lo
	ret

END (__memcmp_aarch64)
