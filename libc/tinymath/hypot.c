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
#include "libc/math.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

#if FLT_EVAL_METHOD > 1U && LDBL_MANT_DIG == 64
#define SPLIT (0x1p32 + 1)
#else
#define SPLIT (0x1p27 + 1)
#endif

static void sq(double_t *hi, double_t *lo, double x)
{
	double_t xh, xl, xc;

	xc = (double_t)x*SPLIT;
	xh = x - xc + xc;
	xl = x - xh;
	*hi = (double_t)x*x;
	*lo = xh*xh - *hi + 2*xh*xl + xl*xl;
}

/**
 * Returns euclidean distance.
 */
double hypot(double x, double y)
{
	union {double f; uint64_t i;} ux = {x}, uy = {y}, ut;
	int ex, ey;
	double_t hx, lx, hy, ly, z;

	/* arrange |x| >= |y| */
	ux.i &= -1ULL>>1;
	uy.i &= -1ULL>>1;
	if (ux.i < uy.i) {
		ut = ux;
		ux = uy;
		uy = ut;
	}

	/* special cases */
	ex = ux.i>>52;
	ey = uy.i>>52;
	x = ux.f;
	y = uy.f;
	/* note: hypot(inf,nan) == inf */
	if (ey == 0x7ff)
		return y;
	if (ex == 0x7ff || uy.i == 0)
		return x;
	/* note: hypot(x,y) ~= x + y*y/x/2 with inexact for small y/x */
	/* 64 difference is enough for ld80 double_t */
	if (ex - ey > 64)
		return x + y;

	/* precise sqrt argument in nearest rounding mode without overflow */
	/* xh*xh must not overflow and xl*xl must not underflow in sq */
	z = 1;
	if (ex > 0x3ff+510) {
		z = 0x1p700;
		x *= 0x1p-700;
		y *= 0x1p-700;
	} else if (ey < 0x3ff-450) {
		z = 0x1p-700;
		x *= 0x1p700;
		y *= 0x1p700;
	}
	sq(&hx, &lx, x);
	sq(&hy, &ly, y);
	return z*sqrt(ly+lx+hy+hx);
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(hypot, hypotl);
#endif
