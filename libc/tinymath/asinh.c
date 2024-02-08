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
#include "libc/tinymath/feval.internal.h"
#include "libc/tinymath/freebsd.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/**
 * Returns inverse hyperbolic sine of 𝑥.
 * @define asinh(x) = sign(x)*log(|x|+sqrt(x*x+1)) ~= x - x^3/6 + o(x^5)
 */
double asinh(double x)
{
	union {double f; uint64_t i;} u = {.f = x};
	unsigned e = u.i >> 52 & 0x7ff;
	unsigned s = u.i >> 63;

	/* |x| */
	u.i &= (uint64_t)-1/2;
	x = u.f;

	if (e >= 0x3ff + 26) {
		/* |x| >= 0x1p26 or inf or nan */
		x = log(x) + 0.693147180559945309417232121458176568;
	} else if (e >= 0x3ff + 1) {
		/* |x| >= 2 */
		x = log(2*x + 1/(sqrt(x*x+1)+x));
	} else if (e >= 0x3ff - 26) {
		/* |x| >= 0x1p-26, up to 1.6ulp error in [0.125,0.5] */
		x = log1p(x + x*x/(sqrt(x*x+1)+1));
	} else {
		/* |x| < 0x1p-26, raise inexact if x != 0 */
		feval(x + 0x1p120f);
	}
	return s ? -x : x;
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(asinh, asinhl);
#endif
