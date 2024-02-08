/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Musl Libc                                                                   │
│  Copyright © 2005-2020 Rich Felker, et al.                                   │
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
FreeBSD libm (BSD-2 License)\\n\
Copyright (c) 2005-2011, Bruce D. Evans, Steven G. Kargl, David Schultz.\"");
asm(".ident\t\"\\n\\n\
fdlibm (fdlibm license)\\n\
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\"");
asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* origin: FreeBSD /usr/src/lib/msun/src/s_cbrtl.c */
/*-
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 * Copyright (c) 2009-2011, Bruce D. Evans, Steven G. Kargl, David Schultz.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 * The argument reduction and testing for exceptional cases was
 * written by Steven G. Kargl with input from Bruce D. Evans
 * and David A. Schultz.
 */

/**
 * Returns cube root of 𝑥.
 */
long double cbrtl(long double x)
{
#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
	return cbrt(x);
#elif (LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113) && LDBL_MAX_EXP == 16384
	static const unsigned B1 = 709958130; /* B1 = (127-127.0/3-0.03306235651)*2**23 */
	union ldshape u = {x}, v;
	union {float f; uint32_t i;} uft;
	long double r, s, t, w;
	double_t dr, dt, dx;
	float_t ft;
	int e = u.i.se & 0x7fff;
	int sign = u.i.se & 0x8000;

	/*
	 * If x = +-Inf, then cbrt(x) = +-Inf.
	 * If x = NaN, then cbrt(x) = NaN.
	 */
	if (e == 0x7fff)
		return x + x;
	if (e == 0) {
		/* Adjust subnormal numbers. */
		u.f *= 0x1p120;
		e = u.i.se & 0x7fff;
		/* If x = +-0, then cbrt(x) = +-0. */
		if (e == 0)
			return x;
		e -= 120;
	}
	e -= 0x3fff;
	u.i.se = 0x3fff;
	x = u.f;
	switch (e % 3) {
	case 1:
	case -2:
		x *= 2;
		e--;
		break;
	case 2:
	case -1:
		x *= 4;
		e -= 2;
		break;
	}
	v.f = 1.0;
	v.i.se = sign | (0x3fff + e/3);

	/*
	 * The following is the guts of s_cbrtf, with the handling of
	 * special values removed and extra care for accuracy not taken,
	 * but with most of the extra accuracy not discarded.
	 */

	/* ~5-bit estimate: */
	uft.f = x;
	uft.i = (uft.i & 0x7fffffff)/3 + B1;
	ft = uft.f;

	/* ~16-bit estimate: */
	dx = x;
	dt = ft;
	dr = dt * dt * dt;
	dt = dt * (dx + dx + dr) / (dx + dr + dr);

	/* ~47-bit estimate: */
	dr = dt * dt * dt;
	dt = dt * (dx + dx + dr) / (dx + dr + dr);

#if LDBL_MANT_DIG == 64
	/*
	 * dt is cbrtl(x) to ~47 bits (after x has been reduced to 1 <= x < 8).
	 * Round it away from zero to 32 bits (32 so that t*t is exact, and
	 * away from zero for technical reasons).
	 */
	t = dt + (0x1.0p32L + 0x1.0p-31L) - 0x1.0p32;
#elif LDBL_MANT_DIG == 113
	/*
	 * Round dt away from zero to 47 bits.  Since we don't trust the 47,
	 * add 2 47-bit ulps instead of 1 to round up.  Rounding is slow and
	 * might be avoidable in this case, since on most machines dt will
	 * have been evaluated in 53-bit precision and the technical reasons
	 * for rounding up might not apply to either case in cbrtl() since
	 * dt is much more accurate than needed.
	 */
	t = dt + 0x2.0p-46 + 0x1.0p60L - 0x1.0p60;
#endif

	/*
	 * Final step Newton iteration to 64 or 113 bits with
	 * error < 0.667 ulps
	 */
	s = t*t;         /* t*t is exact */
	r = x/s;         /* error <= 0.5 ulps; |r| < |t| */
	w = t+t;         /* t+t is exact */
	r = (r-t)/(w+r); /* r-t is exact; w+r ~= 3*t */
	t = t+t*r;       /* error <= 0.5 + 0.5/3 + epsilon */

	t *= v.f;
	return t;

#else
#error "architecture unsupported"
#endif
}
