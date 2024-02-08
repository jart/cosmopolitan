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
#include "libc/tinymath/expo.internal.h"
#include "libc/tinymath/freebsd.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/**
 * Returns hyperbolic sine of 𝑥.
 *
 *     sinh(x) = (exp(x) - 1/exp(x))/2
 *             = (exp(x)-1 + (exp(x)-1)/exp(x))/2
 *             = x + x^3/6 + o(x^5)
 */
double sinh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	uint32_t w;
	double t, h, absx;

	h = 0.5;
	if (u.i >> 63)
		h = -h;
	/* |x| */
	u.i &= (uint64_t)-1/2;
	absx = u.f;
	w = u.i >> 32;

	/* |x| < log(DBL_MAX) */
	if (w < 0x40862e42) {
		t = expm1(absx);
		if (w < 0x3ff00000) {
			if (w < 0x3ff00000 - (26<<20))
				/* note: inexact and underflow are raised by expm1 */
				/* note: this branch avoids spurious underflow */
				return x;
			return h*(2*t - t*t/(t+1));
		}
		/* note: |x|>log(0x1p26)+eps could be just h*exp(x) */
		return h*(t + t/(t+1));
	}

	/* |x| > log(DBL_MAX) or nan */
	/* note: the result is stored to handle overflow */
	t = __expo2(absx, 2*h);
	return t;
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(sinh, sinhl);
#endif
