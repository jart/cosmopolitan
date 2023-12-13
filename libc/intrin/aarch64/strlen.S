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

#define __strlen_aarch64 strlen

.ident "\n\n\
Optimized Routines (MIT License)\n\
Copyright 2022 ARM Limited\n"
.include "libc/disclaimer.inc"

/* Assumptions:
 *
 * ARMv8-a, AArch64, Advanced SIMD, unaligned accesses.
 * Not MTE compatible.
 */

#define srcin	x0
#define len	x0

#define src	x1
#define data1	x2
#define data2	x3
#define has_nul1 x4
#define has_nul2 x5
#define tmp1	x4
#define tmp2	x5
#define tmp3	x6
#define tmp4	x7
#define zeroones x8

#define maskv	v0
#define maskd	d0
#define dataq1	q1
#define dataq2	q2
#define datav1	v1
#define datav2	v2
#define tmp	x2
#define tmpw	w2
#define synd	x3
#define syndw	w3
#define shift	x4

/* For the first 32 bytes, NUL detection works on the principle that
   (X - 1) & (~X) & 0x80 (=> (X - 1) & ~(X | 0x7f)) is non-zero if a
   byte is zero, and can be done in parallel across the entire word.  */

#define REP8_01 0x0101010101010101
#define REP8_7f 0x7f7f7f7f7f7f7f7f

/* To test the page crossing code path more thoroughly, compile with
   -DTEST_PAGE_CROSS - this will force all calls through the slower
   entry path.  This option is not intended for production use.  */

#ifdef TEST_PAGE_CROSS
# define MIN_PAGE_SIZE 32
#else
# define MIN_PAGE_SIZE 4096
#endif

/* Core algorithm:

   Since strings are short on average, we check the first 32 bytes of the
   string for a NUL character without aligning the string.  In order to use
   unaligned loads safely we must do a page cross check first.

   If there is a NUL byte we calculate the length from the 2 8-byte words
   using conditional select to reduce branch mispredictions (it is unlikely
   strlen will be repeatedly called on strings with the same length).

   If the string is longer than 32 bytes, align src so we don't need further
   page cross checks, and process 32 bytes per iteration using a fast SIMD
   loop.

   If the page cross check fails, we read 32 bytes from an aligned address,
   and ignore any characters before the string.  If it contains a NUL
   character, return the length, if not, continue in the main loop.  */

ENTRY (__strlen_aarch64)
	PTR_ARG (0)
	and	tmp1, srcin, MIN_PAGE_SIZE - 1
	cmp	tmp1, MIN_PAGE_SIZE - 32
	b.hi	L(page_cross)

	/* Look for a NUL byte in the first 16 bytes.  */
	ldp	data1, data2, [srcin]
	mov	zeroones, REP8_01

#ifdef __AARCH64EB__
	/* For big-endian, carry propagation (if the final byte in the
	   string is 0x01) means we cannot use has_nul1/2 directly.
	   Since we expect strings to be small and early-exit,
	   byte-swap the data now so has_null1/2 will be correct.  */
	rev	data1, data1
	rev	data2, data2
#endif
	sub	tmp1, data1, zeroones
	orr	tmp2, data1, REP8_7f
	sub	tmp3, data2, zeroones
	orr	tmp4, data2, REP8_7f
	bics	has_nul1, tmp1, tmp2
	bic	has_nul2, tmp3, tmp4
	ccmp	has_nul2, 0, 0, eq
	b.eq	L(bytes16_31)

	/* Find the exact offset of the first NUL byte in the first 16 bytes
	   from the string start.  Enter with C = has_nul1 == 0.  */
	csel	has_nul1, has_nul1, has_nul2, cc
	mov	len, 8
	rev	has_nul1, has_nul1
	csel	len, xzr, len, cc
	clz	tmp1, has_nul1
	add	len, len, tmp1, lsr 3
	ret

	/* Look for a NUL byte at offset 16..31 in the string.  */
L(bytes16_31):
	ldp	data1, data2, [srcin, 16]
#ifdef __AARCH64EB__
	rev	data1, data1
	rev	data2, data2
#endif
	sub	tmp1, data1, zeroones
	orr	tmp2, data1, REP8_7f
	sub	tmp3, data2, zeroones
	orr	tmp4, data2, REP8_7f
	bics	has_nul1, tmp1, tmp2
	bic	has_nul2, tmp3, tmp4
	ccmp	has_nul2, 0, 0, eq
	b.eq	L(loop_entry)

	/* Find the exact offset of the first NUL byte at offset 16..31 from
	   the string start.  Enter with C = has_nul1 == 0.  */
	csel	has_nul1, has_nul1, has_nul2, cc
	mov	len, 24
	rev	has_nul1, has_nul1
	mov	tmp3, 16
	clz	tmp1, has_nul1
	csel	len, tmp3, len, cc
	add	len, len, tmp1, lsr 3
	ret

	nop
L(loop_entry):
	bic	src, srcin, 31

	.p2align 5
L(loop):
	ldp	dataq1, dataq2, [src, 32]!
	uminp	maskv.16b, datav1.16b, datav2.16b
	uminp	maskv.16b, maskv.16b, maskv.16b
	cmeq	maskv.8b, maskv.8b, 0
	fmov	synd, maskd
	cbz	synd, L(loop)

	/* Low 32 bits of synd are non-zero if a NUL was found in datav1.  */
	cmeq	maskv.16b, datav1.16b, 0
	sub	len, src, srcin
	cbnz	syndw, 1f
	cmeq	maskv.16b, datav2.16b, 0
	add	len, len, 16
1:
	/* Generate a bitmask and compute correct byte offset.  */
	shrn	maskv.8b, maskv.8h, 4
	fmov	synd, maskd
#ifndef __AARCH64EB__
	rbit	synd, synd
#endif
	clz	tmp, synd
	add	len, len, tmp, lsr 2
	ret

L(page_cross):
	bic	src, srcin, 31
	mov	tmpw, 0x0c03
	movk	tmpw, 0xc030, lsl 16
	ld1	{datav1.16b, datav2.16b}, [src]
	dup	maskv.4s, tmpw
	cmeq	datav1.16b, datav1.16b, 0
	cmeq	datav2.16b, datav2.16b, 0
	and	datav1.16b, datav1.16b, maskv.16b
	and	datav2.16b, datav2.16b, maskv.16b
	addp	maskv.16b, datav1.16b, datav2.16b
	addp	maskv.16b, maskv.16b, maskv.16b
	fmov	synd, maskd
	lsl	shift, srcin, 1
	lsr	synd, synd, shift
	cbz	synd, L(loop)

	rbit	synd, synd
	clz	len, synd
	lsr	len, len, 1
	ret

END (__strlen_aarch64)
