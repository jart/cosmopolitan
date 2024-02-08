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

/* origin: FreeBSD /usr/src/lib/msun/src/s_csqrt.c */
/*-
 * Copyright (c) 2007 David Schultz <das@FreeBSD.ORG>
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

/*
 * gcc doesn't implement complex multiplication or division correctly,
 * so we need to handle infinities specially. We turn on this pragma to
 * notify conforming c99 compilers that the fast-but-incorrect code that
 * gcc generates is acceptable, since the special cases have already been
 * handled.
 */
// TODO(elimisteve): write test proving why we do / don't need it
// #pragma STDC CX_LIMITED_RANGE ON

/* We risk spurious overflow for components >= DBL_MAX / (1 + sqrt(2)). */
#define THRESH  0x1.a827999fcef32p+1022

/**
 * Returns complex square root.
 */
complex double csqrt(complex double z)
{
	complex double result;
	double a, b;
	double t;
	int scale;

	a = creal(z);
	b = cimag(z);

	/* Handle special cases. */
	if (z == 0)
		return CMPLX(0, b);
	if (isinf(b))
		return CMPLX(INFINITY, b);
	if (isnan(a)) {
		t = (b - b) / (b - b);  /* raise invalid if b is not a NaN */
		return CMPLX(a, t);   /* return NaN + NaN i */
	}
	if (isinf(a)) {
		/*
		 * csqrt(inf + NaN i)  = inf +  NaN i
		 * csqrt(inf + y i)    = inf +  0 i
		 * csqrt(-inf + NaN i) = NaN +- inf i
		 * csqrt(-inf + y i)   = 0   +  inf i
		 */
		if (signbit(a))
			return CMPLX(fabs(b - b), copysign(a, b));
		else
			return CMPLX(a, copysign(b - b, b));
	}
	/*
	 * The remaining special case (b is NaN) is handled just fine by
	 * the normal code path below.
	 */

	/* Scale to avoid overflow. */
	if (fabs(a) >= THRESH || fabs(b) >= THRESH) {
		a *= 0.25;
		b *= 0.25;
		scale = 1;
	} else {
		scale = 0;
	}

	/* Algorithm 312, CACM vol 10, Oct 1967. */
	if (a >= 0) {
		t = sqrt((a + hypot(a, b)) * 0.5);
		result = CMPLX(t, b / (2 * t));
	} else {
		t = sqrt((-a + hypot(a, b)) * 0.5);
		result = CMPLX(fabs(b) / (2 * t), copysign(t, b));
	}

	/* Rescale. */
	if (scale)
		result *= 2;
	return result;
}
