/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2014 Rich Felker, et al.                                   │
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
#include "libc/intrin/likely.h"
#include "libc/math.h"
#include "libc/tinymath/complex.internal.h"
#include "libc/tinymath/internal.h"
#include "libc/tinymath/log2f_data.internal.h"

asm(".ident\t\"\\n\\n\
Double-precision math functions (MIT License)\\n\
Copyright 2018 ARM Limited\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/*
 * Single-precision log2 function.
 *
 * Copyright (c) 2017-2018, Arm Limited.
 * SPDX-License-Identifier: MIT
 */

/*
LOG2F_TABLE_BITS = 4
LOG2F_POLY_ORDER = 4

ULP error: 0.752 (nearest rounding.)
Relative error: 1.9 * 2^-26 (before rounding.)
*/

#define N (1 << LOG2F_TABLE_BITS)
#define T __log2f_data.tab
#define A __log2f_data.poly
#define OFF 0x3f330000

/**
 * Calculates log₂𝑥.
 */
float log2f(float x)
{
	double_t z, r, r2, p, y, y0, invc, logc;
	uint32_t ix, iz, top, tmp;
	int k, i;

	ix = asuint(x);
	/* Fix sign of zero with downward rounding when x==1.  */
	if (WANT_ROUNDING && UNLIKELY(ix == 0x3f800000))
		return 0;
	if (UNLIKELY(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
		/* x < 0x1p-126 or inf or nan.  */
		if (ix * 2 == 0)
			return __math_divzerof(1);
		if (ix == 0x7f800000) /* log2(inf) == inf.  */
			return x;
		if ((ix & 0x80000000) || ix * 2 >= 0xff000000)
			return __math_invalidf(x);
		/* x is subnormal, normalize it.  */
		ix = asuint(x * 0x1p23f);
		ix -= 23 << 23;
	}

	/* x = 2^k z; where z is in range [OFF,2*OFF] and exact.
	   The range is split into N subintervals.
	   The ith subinterval contains z and c is near its center.  */
	tmp = ix - OFF;
	i = (tmp >> (23 - LOG2F_TABLE_BITS)) % N;
	top = tmp & 0xff800000;
	iz = ix - top;
	k = (int32_t)tmp >> 23; /* arithmetic shift */
	invc = T[i].invc;
	logc = T[i].logc;
	z = (double_t)asfloat(iz);

	/* log2(x) = log1p(z/c-1)/ln2 + log2(c) + k */
	r = z * invc - 1;
	y0 = logc + (double_t)k;

	/* Pipelined polynomial evaluation to approximate log1p(r)/ln2.  */
	r2 = r * r;
	y = A[1] * r + A[2];
	y = A[0] * r2 + y;
	p = A[3] * r + y0;
	y = y * r2 + p;
	return eval_as_float(y);
}
