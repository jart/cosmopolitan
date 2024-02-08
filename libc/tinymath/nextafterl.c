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
#include "libc/tinymath/internal.h"
#include "libc/tinymath/ldshape.internal.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

long double nextafterl(long double x, long double y)
{
#if LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
	union ldshape ux, uy;

	if (isunordered(x, y))
		return x + y;
	if (x == y)
		return y;
	ux.f = x;
	if (x == 0) {
		uy.f = y;
		ux.i.m = 1;
		ux.i.se = uy.i.se & 0x8000;
	} else if ((x < y) == !(ux.i.se & 0x8000)) {
		ux.i.m++;
		if (ux.i.m << 1 == 0) {
			ux.i.m = 1ULL << 63;
			ux.i.se++;
		}
	} else {
		if (ux.i.m << 1 == 0) {
			ux.i.se--;
			if (ux.i.se)
				ux.i.m = 0;
		}
		ux.i.m--;
	}
	/* raise overflow if ux is infinite and x is finite */
	if ((ux.i.se & 0x7fff) == 0x7fff)
		return x + x;
	/* raise underflow if ux is subnormal or zero */
	if ((ux.i.se & 0x7fff) == 0)
		FORCE_EVAL(x*x + ux.f*ux.f);
	return ux.f;
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
	union ldshape ux, uy;

	if (isunordered(x, y))
		return x + y;
	if (x == y)
		return y;
	ux.f = x;
	if (x == 0) {
		uy.f = y;
		ux.i.lo = 1;
		ux.i.se = uy.i.se & 0x8000;
	} else if ((x < y) == !(ux.i.se & 0x8000)) {
		ux.i2.lo++;
		if (ux.i2.lo == 0)
			ux.i2.hi++;
	} else {
		if (ux.i2.lo == 0)
			ux.i2.hi--;
		ux.i2.lo--;
	}
	/* raise overflow if ux is infinite and x is finite */
	if ((ux.i.se & 0x7fff) == 0x7fff)
		return x + x;
	/* raise underflow if ux is subnormal or zero */
	if ((ux.i.se & 0x7fff) == 0)
		FORCE_EVAL(x*x + ux.f*ux.f);
	return ux.f;
#endif
}

#endif /* long double is long */
