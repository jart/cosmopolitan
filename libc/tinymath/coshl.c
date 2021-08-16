/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│vi: set et ft=c ts=8 tw=8 fenc=utf-8                                       :vi│
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
#include "libc/tinymath/ldshape.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/**
 * Returns hyperbolic cosine of 𝑥.
 *
 *     cosh(x) = (exp(x) + 1/exp(x))/2
 *             = 1 + 0.5*(exp(x)-1)*(exp(x)-1)/exp(x)
 *             = 1 + x*x/2 + o(x^4)
 */
long double coshl(long double x)
{
	union ldshape u = {x};
	unsigned ex = u.i.se & 0x7fff;
	uint32_t w;
	long double t;

	/* |x| */
	u.i.se = ex;
	x = u.f;
	w = u.i.m >> 32;

	/* |x| < log(2) */
	if (ex < 0x3fff-1 || (ex == 0x3fff-1 && w < 0xb17217f7)) {
		if (ex < 0x3fff-32) {
			fevalf(x + 0x1p120f);
			return 1;
		}
		t = expm1l(x);
		return 1 + t*t/(2*(1+t));
	}

	/* |x| < log(LDBL_MAX) */
	if (ex < 0x3fff+13 || (ex == 0x3fff+13 && w < 0xb17217f7)) {
		t = expl(x);
		return 0.5*(t + 1/t);
	}

	/* |x| > log(LDBL_MAX) or nan */
	t = expl(0.5*x);
	return 0.5*t*t;
}
