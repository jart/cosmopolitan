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
#include "libc/tinymath/ldshape.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/**
 * Returns hyperbolic tangent of 𝑥.
 *
 *     tanh(x) = (exp(x) - exp(-x))/(exp(x) + exp(-x))
 *             = (exp(2*x) - 1)/(exp(2*x) - 1 + 2)
 *             = (1 - exp(-2*x))/(exp(-2*x) - 1 + 2)
 */
long double tanhl(long double x)
{
	union ldshape u = {x};
	unsigned ex = u.i.se & 0x7fff;
	unsigned sign = u.i.se & 0x8000;
	uint32_t w;
	long double t;

	/* x = |x| */
	u.i.se = ex;
	x = u.f;
	w = u.i.m >> 32;

	if (ex > 0x3ffe || (ex == 0x3ffe && w > 0x8c9f53d5)) {
		/* |x| > log(3)/2 ~= 0.5493 or nan */
		if (ex >= 0x3fff+5) {
			/* |x| >= 32 */
			t = 1 + 0/(x + 0x1p-120f);
		} else {
			t = expm1l(2*x);
			t = 1 - 2/(t+2);
		}
	} else if (ex > 0x3ffd || (ex == 0x3ffd && w > 0x82c577d4)) {
		/* |x| > log(5/3)/2 ~= 0.2554 */
		t = expm1l(2*x);
		t = t/(t+2);
	} else {
		/* |x| is small */
		t = expm1l(-2*x);
		t = -t/(t+2);
	}
	return sign ? -t : t;
}
