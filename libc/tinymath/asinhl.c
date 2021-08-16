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
#include "libc/tinymath/feval.internal.h"
#include "libc/tinymath/ldshape.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/**
 * Returns inverse hyperbolic sine of 𝑥.
 * @define asinh(x) = sign(x)*log(|x|+sqrt(x*x+1)) ~= x - x^3/6 + o(x^5)
 */
long double asinhl(long double x)
{
	union ldshape u = {x};
	unsigned e = u.i.se & 0x7fff;
	unsigned s = u.i.se >> 15;

	/* |x| */
	u.i.se = e;
	x = u.f;

	if (e >= 0x3fff + 32) {
		/* |x| >= 0x1p32 or inf or nan */
		x = logl(x) + 0.693147180559945309417232121458176568L;
	} else if (e >= 0x3fff + 1) {
		/* |x| >= 2 */
		x = logl(2*x + 1/(sqrtl(x*x+1)+x));
	} else if (e >= 0x3fff - 32) {
		/* |x| >= 0x1p-32 */
		x = log1pl(x + x*x/(sqrtl(x*x+1)+1));
	} else {
		/* |x| < 0x1p-32, raise inexact if x!=0 */
		fevall(x + 0x1p120f);
	}
	return s ? -x : x;
}
