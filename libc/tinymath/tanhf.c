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
#include "libc/tinymath/internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/**
 * Returns hyperbolic tangent of 𝑥.
 * 
 * @define `tanhf(x)=(expf(x)-expf(-x))/(expf(x)+expf(-x))`
 * @define `tanhf(x)=(expf(2.f*x)-1.f)/(expf(2.f*x)-1.f+2.f)`
 */
float tanhf(float x)
{
	union {float f; uint32_t i;} u = {.f = x};
	uint32_t w;
	int sign;
	float t;

	/* x = |x| */
	sign = u.i >> 31;
	u.i &= 0x7fffffff;
	x = u.f;
	w = u.i;

	if (w > 0x3f0c9f54) {
		/* |x| > log(3)/2 ~= 0.5493 or nan */
		if (w > 0x41200000) {
			/* |x| > 10 */
			t = 1 + 0/x;
		} else {
			t = expm1f(2*x);
			t = 1 - 2/(t+2);
		}
	} else if (w > 0x3e82c578) {
		/* |x| > log(5/3)/2 ~= 0.2554 */
		t = expm1f(2*x);
		t = t/(t+2);
	} else if (w >= 0x00800000) {
		/* |x| >= 0x1p-126 */
		t = expm1f(-2*x);
		t = -t/(t+2);
	} else {
		/* |x| is subnormal */
		FORCE_EVAL(x*x);
		t = x;
	}
	return sign ? -t : t;
}
