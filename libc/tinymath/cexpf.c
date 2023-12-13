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
#include "libc/complex.h"
#include "libc/math.h"
#include "libc/tinymath/complex.internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */


/* origin: FreeBSD /usr/src/lib/msun/src/s_cexpf.c */
/*-
 * Copyright (c) 2011 David Schultz <das@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


static const uint32_t
exp_ovfl  = 0x42b17218,  /* MAX_EXP * ln2 ~= 88.722839355 */
cexp_ovfl = 0x43400074;  /* (MAX_EXP - MIN_DENORM_EXP) * ln2 */

float complex cexpf(float complex z)
{
	float x, y, exp_x;
	uint32_t hx, hy;

	x = crealf(z);
	y = cimagf(z);

	GET_FLOAT_WORD(hy, y);
	hy &= 0x7fffffff;

	/* cexp(x + I 0) = exp(x) + I 0 */
	if (hy == 0)
		return CMPLXF(expf(x), y);
	GET_FLOAT_WORD(hx, x);
	/* cexp(0 + I y) = cos(y) + I sin(y) */
	if ((hx & 0x7fffffff) == 0)
		return CMPLXF(cosf(y), sinf(y));

	if (hy >= 0x7f800000) {
		if ((hx & 0x7fffffff) != 0x7f800000) {
			/* cexp(finite|NaN +- I Inf|NaN) = NaN + I NaN */
			return CMPLXF(y - y, y - y);
		} else if (hx & 0x80000000) {
			/* cexp(-Inf +- I Inf|NaN) = 0 + I 0 */
			return CMPLXF(0.0, 0.0);
		} else {
			/* cexp(+Inf +- I Inf|NaN) = Inf + I NaN */
			return CMPLXF(x, y - y);
		}
	}

	if (hx >= exp_ovfl && hx <= cexp_ovfl) {
		/*
		 * x is between 88.7 and 192, so we must scale to avoid
		 * overflow in expf(x).
		 */
		return __ldexp_cexpf(z, 0);
	} else {
		/*
		 * Cases covered here:
		 *  -  x < exp_ovfl and exp(x) won't overflow (common case)
		 *  -  x > cexp_ovfl, so exp(x) * s overflows for all s > 0
		 *  -  x = +-Inf (generated by exp())
		 *  -  x = NaN (spurious inexact exception from y)
		 */
		exp_x = expf(x);
		return CMPLXF(exp_x * cosf(y), exp_x * sinf(y));
	}
}
