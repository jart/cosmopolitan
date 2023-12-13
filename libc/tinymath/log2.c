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
#include "libc/tinymath/log2_data.internal.h"

asm(".ident\t\"\\n\\n\
Double-precision math functions (MIT License)\\n\
Copyright 2018 ARM Limited\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/*
 * Double-precision log2(x) function.
 *
 * Copyright (c) 2018, Arm Limited.
 * SPDX-License-Identifier: MIT
 */

#define T __log2_data.tab
#define T2 __log2_data.tab2
#define B __log2_data.poly1
#define A __log2_data.poly
#define InvLn2hi __log2_data.invln2hi
#define InvLn2lo __log2_data.invln2lo
#define N (1 << LOG2_TABLE_BITS)
#define OFF 0x3fe6000000000000

/* Top 16 bits of a double.  */
static inline uint32_t top16(double x)
{
	return asuint64(x) >> 48;
}

/**
 * Calculates log₂𝑥.
 */
double log2(double x)
{
	double_t z, r, r2, r4, y, invc, logc, kd, hi, lo, t1, t2, t3, p;
	uint64_t ix, iz, tmp;
	uint32_t top;
	int k, i;

	ix = asuint64(x);
	top = top16(x);
#define LO asuint64(1.0 - 0x1.5b51p-5)
#define HI asuint64(1.0 + 0x1.6ab2p-5)
	if (UNLIKELY(ix - LO < HI - LO)) {
		/* Handle close to 1.0 inputs separately.  */
		/* Fix sign of zero with downward rounding when x==1.  */
		if (WANT_ROUNDING && UNLIKELY(ix == asuint64(1.0)))
			return 0;
		r = x - 1.0;
#if __FP_FAST_FMA
		hi = r * InvLn2hi;
		lo = r * InvLn2lo + __builtin_fma(r, InvLn2hi, -hi);
#else
		double_t rhi, rlo;
		rhi = asdouble(asuint64(r) & -1ULL << 32);
		rlo = r - rhi;
		hi = rhi * InvLn2hi;
		lo = rlo * InvLn2hi + r * InvLn2lo;
#endif
		r2 = r * r; /* rounding error: 0x1p-62.  */
		r4 = r2 * r2;
		/* Worst-case error is less than 0.54 ULP (0.55 ULP without fma).  */
		p = r2 * (B[0] + r * B[1]);
		y = hi + p;
		lo += hi - y + p;
		lo += r4 * (B[2] + r * B[3] + r2 * (B[4] + r * B[5]) +
			    r4 * (B[6] + r * B[7] + r2 * (B[8] + r * B[9])));
		y += lo;
		return eval_as_double(y);
	}
	if (UNLIKELY(top - 0x0010 >= 0x7ff0 - 0x0010)) {
		/* x < 0x1p-1022 or inf or nan.  */
		if (ix * 2 == 0)
			return __math_divzero(1);
		if (ix == asuint64(INFINITY)) /* log(inf) == inf.  */
			return x;
		if ((top & 0x8000) || (top & 0x7ff0) == 0x7ff0)
			return __math_invalid(x);
		/* x is subnormal, normalize it.  */
		ix = asuint64(x * 0x1p52);
		ix -= 52ULL << 52;
	}

	/* x = 2^k z; where z is in range [OFF,2*OFF) and exact.
	   The range is split into N subintervals.
	   The ith subinterval contains z and c is near its center.  */
	tmp = ix - OFF;
	i = (tmp >> (52 - LOG2_TABLE_BITS)) % N;
	k = (int64_t)tmp >> 52; /* arithmetic shift */
	iz = ix - (tmp & 0xfffULL << 52);
	invc = T[i].invc;
	logc = T[i].logc;
	z = asdouble(iz);
	kd = (double_t)k;

	/* log2(x) = log2(z/c) + log2(c) + k.  */
	/* r ~= z/c - 1, |r| < 1/(2*N).  */
#if __FP_FAST_FMA
	/* rounding error: 0x1p-55/N.  */
	r = __builtin_fma(z, invc, -1.0);
	t1 = r * InvLn2hi;
	t2 = r * InvLn2lo + __builtin_fma(r, InvLn2hi, -t1);
#else
	double_t rhi, rlo;
	/* rounding error: 0x1p-55/N + 0x1p-65.  */
	r = (z - T2[i].chi - T2[i].clo) * invc;
	rhi = asdouble(asuint64(r) & -1ULL << 32);
	rlo = r - rhi;
	t1 = rhi * InvLn2hi;
	t2 = rlo * InvLn2hi + r * InvLn2lo;
#endif

	/* hi + lo = r/ln2 + log2(c) + k.  */
	t3 = kd + logc;
	hi = t3 + t1;
	lo = t3 - hi + t1 + t2;

	/* log2(r+1) = r/ln2 + r^2*poly(r).  */
	/* Evaluation is optimized assuming superscalar pipelined execution.  */
	r2 = r * r; /* rounding error: 0x1p-54/N^2.  */
	r4 = r2 * r2;
	/* Worst-case error if |y| > 0x1p-4: 0.547 ULP (0.550 ULP without fma).
	   ~ 0.5 + 2/N/ln2 + abs-poly-error*0x1p56 ULP (+ 0.003 ULP without fma).  */
	p = A[0] + r * A[1] + r2 * (A[2] + r * A[3]) + r4 * (A[4] + r * A[5]);
	y = lo + r2 * p + hi;
	return eval_as_double(y);
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(log2, log2l);
#endif
