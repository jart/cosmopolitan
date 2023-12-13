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

/* origin: FreeBSD /usr/src/lib/msun/src/e_acosl.c */
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
 * See comments in acos.c.
 * Converted to long double by David Schultz <das@FreeBSD.ORG>.
 */

#if LDBL_MANT_DIG == 64
#define CLEARBOTTOM(u) (u.i.m &= -1ULL << 32)
#elif LDBL_MANT_DIG == 113
#define CLEARBOTTOM(u) (u.i.lo = 0)
#endif

/**
 * Returns arc cosine of 𝑥.
 *
 * @define atan2(fabs(sqrt((1-𝑥)*(1+𝑥))),𝑥)
 * @domain -1 ≤ 𝑥 ≤ 1
 */
long double acosl(long double x)
{
	union ldshape u = {x};
	long double z, s, c, f;
	uint16_t e = u.i.se & 0x7fff;

	/* |x| >= 1 or nan */
	if (e >= 0x3fff) {
		if (x == 1)
			return 0;
		if (x == -1)
			return 2*pio2_hi + 0x1p-120f;
		return 0/(x-x);
	}
	/* |x| < 0.5 */
	if (e < 0x3fff - 1) {
		if (e < 0x3fff - LDBL_MANT_DIG - 1)
			return pio2_hi + 0x1p-120f;
		return pio2_hi - (__invtrigl_R(x*x)*x - pio2_lo + x);
	}
	/* x < -0.5 */
	if (u.i.se >> 15) {
		z = (1 + x)*0.5;
		s = sqrtl(z);
		return 2*(pio2_hi - (__invtrigl_R(z)*s - pio2_lo + s));
	}
	/* x > 0.5 */
	z = (1 - x)*0.5;
	s = sqrtl(z);
	u.f = s;
	CLEARBOTTOM(u);
	f = u.f;
	c = (z - f*f)/(s + f);
	return 2*(__invtrigl_R(z)*s + c + f);
}

#endif /* long double is long */
