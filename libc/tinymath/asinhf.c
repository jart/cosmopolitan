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

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

/**
 * Returns inverse hyperbolic sine of 𝑥.
 * @define asinh(x) = sign(x)*log(|x|+sqrt(x*x+1)) ~= x - x^3/6 + o(x^5)
 */
float asinhf(float x)
{
	union {float f; uint32_t i;} u = {.f = x};
	uint32_t i = u.i & 0x7fffffff;
	unsigned s = u.i >> 31;

	/* |x| */
	u.i = i;
	x = u.f;

	if (i >= 0x3f800000 + (12<<23)) {
		/* |x| >= 0x1p12 or inf or nan */
		x = logf(x) + 0.693147180559945309417232121458176568f;
	} else if (i >= 0x3f800000 + (1<<23)) {
		/* |x| >= 2 */
		x = logf(2*x + 1/(sqrtf(x*x+1)+x));
	} else if (i >= 0x3f800000 - (12<<23)) {
		/* |x| >= 0x1p-12, up to 1.6ulp error in [0.125,0.5] */
		x = log1pf(x + x*x/(sqrtf(x*x+1)+1));
	} else {
		/* |x| < 0x1p-12, raise inexact if x!=0 */
		feval(x + 0x1p120f);
	}
	return s ? -x : x;
}
