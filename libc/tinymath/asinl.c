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
#include "libc/tinymath/invtrigl.internal.h"
#include "libc/tinymath/ldshape.internal.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

asm(".ident\t\"\\n\\n\
fdlibm (fdlibm license)\\n\
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\"");
asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* origin: FreeBSD /usr/src/lib/msun/src/e_asinl.c */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */
/*
 * See comments in asin.c.
 * Converted to long double by David Schultz <das@FreeBSD.ORG>.
 */

#if LDBL_MANT_DIG == 64
#define CLOSETO1(u) (u.i.m>>56 >= 0xf7)
#define CLEARBOTTOM(u) (u.i.m &= -1ULL << 32)
#elif LDBL_MANT_DIG == 113
#define CLOSETO1(u) (u.i.top >= 0xee00)
#define CLEARBOTTOM(u) (u.i.lo = 0)
#endif

/**
 * Returns arc sine of 𝑥.
 *
 * @define atan2(𝑥,sqrt((1-𝑥)*(1+𝑥)))
 * @domain -1 ≤ 𝑥 ≤ 1
 */
long double asinl(long double x)
{
	union ldshape u = {x};
	long double z, r, s;
	uint16_t e = u.i.se & 0x7fff;
	int sign = u.i.se >> 15;

	if (e >= 0x3fff) {   /* |x| >= 1 or nan */
		/* asin(+-1)=+-pi/2 with inexact */
		if (x == 1 || x == -1)
			return x*pio2_hi + 0x1p-120f;
		return 0/(x-x);
	}
	if (e < 0x3fff - 1) {  /* |x| < 0.5 */
		if (e < 0x3fff - (LDBL_MANT_DIG+1)/2) {
			/* return x with inexact if x!=0 */
			FORCE_EVAL(x + 0x1p120f);
			return x;
		}
		return x + x*__invtrigl_R(x*x);
	}
	/* 1 > |x| >= 0.5 */
	z = (1.0 - fabsl(x))*0.5;
	s = sqrtl(z);
	r = __invtrigl_R(z);
	if (CLOSETO1(u)) {
		x = pio2_hi - (2*(s+s*r)-pio2_lo);
	} else {
		long double f, c;
		u.f = s;
		CLEARBOTTOM(u);
		f = u.f;
		c = (z - f*f)/(s + f);
		x = 0.5*pio2_hi-(2*s*r - (pio2_lo-2*c) - (0.5*pio2_hi-2*f));
	}
	return sign ? -x : x;

}

#endif /* long double is long */
