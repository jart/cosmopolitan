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

#define __strchr_aarch64 strchr
#define __index_aarch64 index

.ident "\n\n\
Optimized Routines (MIT License)\n\
Copyright 2022 ARM Limited\n"
.include "libc/disclaimer.inc"

/* Assumptions:
 *
 * ARMv8-a, AArch64
 * Neon Available.
 */

/* Arguments and results.  */
#define srcin		x0
#define chrin		w1

#define result		x0

#define src		x2
#define	tmp1		x3
#define wtmp2		w4
#define tmp3		x5

#define vrepchr		v0
#define vdata1		v1
#define vdata2		v2
#define vhas_nul1	v3
#define vhas_nul2	v4
#define vhas_chr1	v5
#define vhas_chr2	v6
#define vrepmask_0	v7
#define vrepmask_c	v16
#define vend1		v17
#define vend2		v18

/* Core algorithm.

   For each 32-byte hunk we calculate a 64-bit syndrome value, with
   two bits per byte (LSB is always in bits 0 and 1, for both big
   and little-endian systems).  For each tuple, bit 0 is set iff
   the relevant byte matched the requested character; bit 1 is set
   iff the relevant byte matched the NUL end of string (we trigger
   off bit0 for the special case of looking for NUL).  Since the bits
   in the syndrome reflect exactly the order in which things occur
   in the original string a count_trailing_zeros() operation will
   identify exactly which byte is causing the termination, and why.  */

/* Locals and temporaries.  */

ENTRY_ALIAS (__strchr_aarch64, __index_aarch64)
ENTRY (__strchr_aarch64)
	PTR_ARG (0)
	/* Magic constant 0xc0300c03 to allow us to identify which lane
	   matches the requested byte.  Even bits are set if the character
	   matches, odd bits if either the char is NUL or matches.  */
	mov	wtmp2, 0x0c03
	movk	wtmp2, 0xc030, lsl 16
	dup	vrepchr.16b, chrin
	bic	src, srcin, #31		/* Work with aligned 32-byte hunks.  */
	dup	vrepmask_c.4s, wtmp2
	ands	tmp1, srcin, #31
	add	vrepmask_0.4s, vrepmask_c.4s, vrepmask_c.4s /* equiv: lsl #1 */
	b.eq	L(loop)

	/* Input string is not 32-byte aligned.  Rather than forcing
	   the padding bytes to a safe value, we calculate the syndrome
	   for all the bytes, but then mask off those bits of the
	   syndrome that are related to the padding.  */
	ld1	{vdata1.16b, vdata2.16b}, [src], #32
	neg	tmp1, tmp1
	cmeq	vhas_nul1.16b, vdata1.16b, #0
	cmeq	vhas_chr1.16b, vdata1.16b, vrepchr.16b
	cmeq	vhas_nul2.16b, vdata2.16b, #0
	cmeq	vhas_chr2.16b, vdata2.16b, vrepchr.16b
	bif	vhas_nul1.16b, vhas_chr1.16b, vrepmask_0.16b
	bif	vhas_nul2.16b, vhas_chr2.16b, vrepmask_0.16b
	and	vend1.16b, vhas_nul1.16b, vrepmask_c.16b
	and	vend2.16b, vhas_nul2.16b, vrepmask_c.16b
	lsl	tmp1, tmp1, #1
	addp	vend1.16b, vend1.16b, vend2.16b		// 256->128
	mov	tmp3, #~0
	addp	vend1.16b, vend1.16b, vend2.16b		// 128->64
	lsr	tmp1, tmp3, tmp1

	mov	tmp3, vend1.d[0]
	bic	tmp1, tmp3, tmp1	// Mask padding bits.
	cbnz	tmp1, L(tail)

	.p2align 4
L(loop):
	ld1	{vdata1.16b, vdata2.16b}, [src], #32
	cmeq	vhas_chr1.16b, vdata1.16b, vrepchr.16b
	cmeq	vhas_chr2.16b, vdata2.16b, vrepchr.16b
	cmhs	vhas_nul1.16b, vhas_chr1.16b, vdata1.16b
	cmhs	vhas_nul2.16b, vhas_chr2.16b, vdata2.16b
	orr	vend1.16b, vhas_nul1.16b, vhas_nul2.16b
	umaxp	vend1.16b, vend1.16b, vend1.16b
	mov	tmp1, vend1.d[0]
	cbz	tmp1, L(loop)

	/* Termination condition found.  Now need to establish exactly why
	   we terminated.  */
	bif	vhas_nul1.16b, vhas_chr1.16b, vrepmask_0.16b
	bif	vhas_nul2.16b, vhas_chr2.16b, vrepmask_0.16b
	and	vend1.16b, vhas_nul1.16b, vrepmask_c.16b
	and	vend2.16b, vhas_nul2.16b, vrepmask_c.16b
	addp	vend1.16b, vend1.16b, vend2.16b		// 256->128
	addp	vend1.16b, vend1.16b, vend2.16b		// 128->64
	mov	tmp1, vend1.d[0]
L(tail):
	/* Count the trailing zeros, by bit reversing...  */
	rbit	tmp1, tmp1
	/* Re-bias source.  */
	sub	src, src, #32
	clz	tmp1, tmp1	/* And counting the leading zeros.  */
	/* Tmp1 is even if the target charager was found first.  Otherwise
	   we've found the end of string and we weren't looking for NUL.  */
	tst	tmp1, #1
	add	result, src, tmp1, lsr #1
	csel	result, result, xzr, eq
	ret

END (__strchr_aarch64)
