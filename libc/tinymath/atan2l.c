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

/* origin: FreeBSD /usr/src/lib/msun/src/e_atan2l.c */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 *
 */
/*
 * See comments in atan2.c.
 * Converted to long double by David Schultz <das@FreeBSD.ORG>.
 */

/**
 * Returns arc tangent of 𝑦/𝑥.
 */
long double atan2l(long double y, long double x)
{
#ifdef __x86__

	asm("fpatan"
	    : "=t"(x)
	    : "0"(x), "u"(y)
	    : "st(1)");
	return x;

#else

	union ldshape ux, uy;
	long double z;
	int m, ex, ey;

	if (isunordered(x, y))
		return x+y;
	if (x == 1)
		return atanl(y);
	ux.f = x;
	uy.f = y;
	ex = ux.i.se & 0x7fff;
	ey = uy.i.se & 0x7fff;
	m = 2*(ux.i.se>>15) | uy.i.se>>15;
	if (y == 0) {
		switch(m) {
		case 0:
		case 1: return y;           /* atan(+-0,+anything)=+-0 */
		case 2: return  2*pio2_hi;  /* atan(+0,-anything) = pi */
		case 3: return -2*pio2_hi;  /* atan(-0,-anything) =-pi */
		}
	}
	if (x == 0)
		return m&1 ? -pio2_hi : pio2_hi;
	if (ex == 0x7fff) {
		if (ey == 0x7fff) {
			switch(m) {
			case 0: return  pio2_hi/2;   /* atan(+INF,+INF) */
			case 1: return -pio2_hi/2;   /* atan(-INF,+INF) */
			case 2: return  1.5*pio2_hi; /* atan(+INF,-INF) */
			case 3: return -1.5*pio2_hi; /* atan(-INF,-INF) */
			}
		} else {
			switch(m) {
			case 0: return  0.0;        /* atan(+...,+INF) */
			case 1: return -0.0;        /* atan(-...,+INF) */
			case 2: return  2*pio2_hi;  /* atan(+...,-INF) */
			case 3: return -2*pio2_hi;  /* atan(-...,-INF) */
			}
		}
	}
	if (ex+120 < ey || ey == 0x7fff)
		return m&1 ? -pio2_hi : pio2_hi;
	/* z = atan(|y/x|) without spurious underflow */
	if ((m&2) && ey+120 < ex)  /* |y/x| < 0x1p-120, x<0 */
		z = 0.0;
	else
		z = atanl(fabsl(y/x));
	switch (m) {
	case 0: return z;               /* atan(+,+) */
	case 1: return -z;              /* atan(-,+) */
	case 2: return 2*pio2_hi-(z-2*pio2_lo); /* atan(+,-) */
	default: /* case 3 */
		return (z-2*pio2_lo)-2*pio2_hi; /* atan(-,-) */
	}

#endif
}

#endif /* long double is long */
