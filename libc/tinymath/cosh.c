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
#include "libc/tinymath/feval.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/**
 * Returns hyperbolic cosine of 𝑥.
 *
 *     cosh(x) = (exp(x) + 1/exp(x))/2
 *             = 1 + 0.5*(exp(x)-1)*(exp(x)-1)/exp(x)
 *             = 1 + x*x/2 + o(x^4)
 */
double cosh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	uint32_t w;
	double t;

	/* |x| */
	u.i &= (uint64_t)-1/2;
	x = u.f;
	w = u.i >> 32;

	/* |x| < log(2) */
	if (w < 0x3fe62e42) {
		if (w < 0x3ff00000 - (26<<20)) {
			/* raise inexact if x!=0 */
			feval(x + 0x1p120f);
			return 1;
		}
		t = expm1(x);
		return 1 + t*t/(2*(1+t));
	}

	/* |x| < log(DBL_MAX) */
	if (w < 0x40862e42) {
		t = exp(x);
		/* note: if x>log(0x1p26) then the 1/t is not needed */
		return 0.5*(t + 1/t);
	}

	/* |x| > log(DBL_MAX) or nan */
	/* note: the result is stored to handle overflow */
	t = __expo2(x, 1.0);
	return t;
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(cosh, coshl);
#endif
