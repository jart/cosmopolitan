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
#include "libc/limits.h"
#include "libc/math.h"
#include "libc/tinymath/internal.h"
#include "libc/tinymath/ldshape.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
int ilogbl(long double x)
{
	return ilogb(x);
}
#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
int ilogbl(long double x)
{
	//#pragma STDC FENV_ACCESS ON
	union ldshape u = {x};
	uint64_t m = u.i.m;
	int e = u.i.se & 0x7fff;

	if (!e) {
		if (m == 0) {
			FORCE_EVAL(0/0.0f);
			return FP_ILOGB0;
		}
		/* subnormal x */
		for (e = -0x3fff+1; m>>63 == 0; e--, m<<=1);
		return e;
	}
	if (e == 0x7fff) {
		FORCE_EVAL(0/0.0f);
		return m<<1 ? FP_ILOGBNAN : INT_MAX;
	}
	return e - 0x3fff;
}
#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
int ilogbl(long double x)
{
	//#pragma STDC FENV_ACCESS ON
	union ldshape u = {x};
	int e = u.i.se & 0x7fff;

	if (!e) {
		if (x == 0) {
			FORCE_EVAL(0/0.0f);
			return FP_ILOGB0;
		}
		/* subnormal x */
		x *= 0x1p120;
		return ilogbl(x) - 120;
	}
	if (e == 0x7fff) {
		FORCE_EVAL(0/0.0f);
		u.i.se = 0;
		return u.f ? FP_ILOGBNAN : INT_MAX;
	}
	return e - 0x3fff;
}
#endif
