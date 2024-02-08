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
#include "libc/runtime/runtime.h"
#include "libc/tinymath/feval.internal.h"
#include "libc/tinymath/kernel.internal.h"

asm(".ident\t\"\\n\\n\
fdlibm (fdlibm license)\\n\
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\"");
asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* origin: FreeBSD /usr/src/lib/msun/src/s_sin.c */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

#define asuint64(f) ((union{double _f; uint64_t _i;}){f})._i
#define gethighw(hi,d) (hi) = asuint64(d) >> 32

/**
 * Returns sine and cosine of 𝑥.
 * @note should take ~10ns
 */
void sincos(double x, double *sin, double *cos)
{
	double y[2], s, c;
	uint32_t ix;
	unsigned n;

	gethighw(ix, x);
	ix &= 0x7fffffff;

	/* |x| ~< pi/4 */
	if (ix <= 0x3fe921fb) {
		/* if |x| < 2**-27 * sqrt(2) */
		if (ix < 0x3e46a09e) {
			/* raise inexact if x!=0 and underflow if subnormal */
			feval(ix < 0x00100000 ? x/0x1p120f : x+0x1p120f);
			*sin = x;
			*cos = 1.0;
			return;
		}
		*sin = __sin(x, 0.0, 0);
		*cos = __cos(x, 0.0);
		return;
	}

	/* sincos(Inf or NaN) is NaN */
	if (ix >= 0x7ff00000) {
		*sin = *cos = x - x;
		return;
	}

	/* argument reduction needed */
	n = __rem_pio2(x, y);
	s = __sin(y[0], y[1], 1);
	c = __cos(y[0], y[1]);
	switch (n&3) {
	case 0:
		*sin = s;
		*cos = c;
		break;
	case 1:
		*sin = c;
		*cos = -s;
		break;
	case 2:
		*sin = -s;
		*cos = -c;
		break;
	case 3:
	default:
		*sin = -c;
		*cos = s;
		break;
	}
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(sincos, sincosl);
#endif
