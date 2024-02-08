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
#include "libc/tinymath/ldshape.internal.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/**
 * Returns largest integral value not greater than 𝑥.
 */
long double floorl(long double x)
{
	static const long double toint = 1/LDBL_EPSILON;

	union ldshape u = {x};
	int e = u.i.se & 0x7fff;
	long double y;

	if (e >= 0x3fff+LDBL_MANT_DIG-1 || x == 0)
		return x;
	/* y = int(x) - x, where int(x) is an integer neighbor of x */
	if (u.i.se >> 15)
		y = x - toint + toint - x;
	else
		y = x + toint - toint - x;
	/* special case because of non-nearest rounding modes */
	if (e <= 0x3fff-1) {
		FORCE_EVAL(y);
		return u.i.se >> 15 ? -1 : 0;
	}
	if (y > 0)
		return x + y - 1;
	return x + y;
}

#endif /* long double is long */
