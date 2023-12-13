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
#include "libc/tinymath/ldshape.internal.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

#if LDBL_MANT_DIG == 64
#define SPLIT (0x1p32L+1)
#elif LDBL_MANT_DIG == 113
#define SPLIT (0x1p57L+1)
#endif

static void sq(long double *hi, long double *lo, long double x)
{
	long double xh, xl, xc;
	xc = x*SPLIT;
	xh = x - xc + xc;
	xl = x - xh;
	*hi = x*x;
	*lo = xh*xh - *hi + 2*xh*xl + xl*xl;
}

/**
 * Returns euclidean distance.
 */
long double hypotl(long double x, long double y)
{
	union ldshape ux = {x}, uy = {y};
	int ex, ey;
	long double hx, lx, hy, ly, z;

	ux.i.se &= 0x7fff;
	uy.i.se &= 0x7fff;
	if (ux.i.se < uy.i.se) {
		ex = uy.i.se;
		ey = ux.i.se;
		x = uy.f;
		y = ux.f;
	} else {
		ex = ux.i.se;
		ey = uy.i.se;
		x = ux.f;
		y = uy.f;
	}

	if (ex == 0x7fff && isinf(y))
		return y;
	if (ex == 0x7fff || y == 0)
		return x;
	if (ex - ey > LDBL_MANT_DIG)
		return x + y;

	z = 1;
	if (ex > 0x3fff+8000) {
		z = 0x1p10000L;
		x *= 0x1p-10000L;
		y *= 0x1p-10000L;
	} else if (ey < 0x3fff-8000) {
		z = 0x1p-10000L;
		x *= 0x1p10000L;
		y *= 0x1p10000L;
	}
	sq(&hx, &lx, x);
	sq(&hy, &ly, y);
	return z*sqrtl(ly+lx+hy+hx);
}

#endif /* long double is long */
