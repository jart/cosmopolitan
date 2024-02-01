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
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/fenv.h"

asm(".ident\t\"\\n\\n\
Fused Multiply Add (MIT License)\\n\
Copyright (c) 2005-2011 David Schultz <das@FreeBSD.ORG>\"");
asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* origin: FreeBSD /usr/src/lib/msun/src/s_fmaf.c */
/*-
 * Copyright (c) 2005-2011 David Schultz <das@FreeBSD.ORG>
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

/**
 * Performs fused multiply add.
 *
 * @return `𝑥 * 𝑦 + 𝑧` with a single rounding error
 */
float fmaf(float x, float y, float z)
{
#if defined(__x86_64__) && defined(__FMA__)

	// Intel Haswell+ (c. 2013)
	// AMD Piledriver+ (c. 2011)
	asm("vfmadd132ss\t%1,%2,%0" : "+x"(x) : "x"(y), "x"(z));
	return x;

#elif defined(__x86_64__) && defined(__FMA4__)

	// AMD Bulldozer+ (c. 2011)
	asm("vfmaddss\t%3,%2,%1,%0" : "=x"(x) : "x"(x), "x"(y), "x"(z));
	return x;

#elif defined(__aarch64__)

	asm("fmadd\t%s0,%s1,%s2,%s3" : "=w"(x) : "w"(x), "w"(y), "w"(z));
	return x;

#elif defined(__powerpc64__)

	asm("fmadds\t%0,%1,%2,%3" : "=f"(x) : "f"(x), "f"(y), "f"(z));
	return x;

#elif defined(__riscv) && __riscv_flen >= 32

	asm("fmadd.s\t%0,%1,%2,%3" : "=f"(x) : "f"(x), "f"(y), "f"(z));
	return x;

#elif defined(__s390x__)

	asm("maebr\t%0,%1,%2" : "+f"(z) : "f"(x), "f"(y));
	return z;

#else

	/* A double has more than twice as much precision than a float,
	   so direct double-precision arithmetic suffices, except where
	   double rounding occurs. */

/* #pragma STDC FENV_ACCESS ON */
	double xy, result;
	union {double f; uint64_t i;} u;
	int e;

	xy = (double)x * y;
	result = xy + z;
	u.f = result;
	e = u.i>>52 & 0x7ff;
	/* Common case: The double precision result is fine. */
	if ((u.i & 0x1fffffff) != 0x10000000 || /* not a halfway case */
		e == 0x7ff ||                   /* NaN */
		(result - xy == z && result - z == xy) || /* exact */
		fegetround() != FE_TONEAREST)       /* not round-to-nearest */
	{
		/*
		underflow may not be raised correctly, example:
		fmaf(0x1p-120f, 0x1p-120f, 0x1p-149f)
		*/
#if defined(FE_INEXACT) && defined(FE_UNDERFLOW)
		if (e < 0x3ff-126 && e >= 0x3ff-149 && fetestexcept(FE_INEXACT)) {
			feclearexcept(FE_INEXACT);
			/* TODO: gcc and clang bug workaround */
			volatile float vz = z;
			result = xy + vz;
			if (fetestexcept(FE_INEXACT))
				feraiseexcept(FE_UNDERFLOW);
			else
				feraiseexcept(FE_INEXACT);
		}
#endif
		z = result;
		return z;
	}

	/*
	 * If result is inexact, and exactly halfway between two float values,
	 * we need to adjust the low-order bit in the direction of the error.
	 */
	double err;
	int neg = u.i >> 63;
	if (neg == (z > xy))
		err = xy - result + z;
	else
		err = z - result + xy;
	if (neg == (err < 0))
		u.i++;
	else
		u.i--;
	z = u.f;
	return z;

#endif /* __x86_64__ */
}
