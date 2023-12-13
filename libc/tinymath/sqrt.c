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
#include "libc/intrin/likely.h"
#include "libc/math.h"
#include "libc/tinymath/internal.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

#define FENV_SUPPORT 1

/* returns a*b*2^-32 - e, with error 0 <= e < 1.  */
static inline uint32_t mul32(uint32_t a, uint32_t b)
{
	return (uint64_t)a*b >> 32;
}

/* returns a*b*2^-64 - e, with error 0 <= e < 3.  */
static inline uint64_t mul64(uint64_t a, uint64_t b)
{
	uint64_t ahi = a>>32;
	uint64_t alo = a&0xffffffff;
	uint64_t bhi = b>>32;
	uint64_t blo = b&0xffffffff;
	return ahi*bhi + (ahi*blo >> 32) + (alo*bhi >> 32);
}

/**
 * Returns square root of 𝑥.
 */
double sqrt(double x)
{
#if defined(__x86_64__) && defined(__SSE2__)

	asm("sqrtsd\t%1,%0" : "=x"(x) : "x"(x));
	return x;

#elif defined(__aarch64__)

	asm("fsqrt\t%d0,%d1" : "=w"(x) : "w"(x));
	return x;

#elif defined(__powerpc64__)

	asm("fsqrt\t%0,%1" : "=d"(x) : "d"(x));
	return x;

#elif defined(__riscv) && __riscv_flen >= 64

	asm("fsqrt.d\t%0,%1" : "=f"(x) : "f"(x));
	return x;

#elif defined(__s390x__) && (defined(__HTM__) || __ARCH__ >= 9)

	asm("sqdbr\t%0,%1" : "=f"(x) : "f"(x));
	return x;

#else

	uint64_t ix, top, m;

	/* special case handling.  */
	ix = asuint64(x);
	top = ix >> 52;
	if (UNLIKELY(top - 0x001 >= 0x7ff - 0x001)) {
		/* x < 0x1p-1022 or inf or nan.  */
		if (ix * 2 == 0)
			return x;
		if (ix == 0x7ff0000000000000)
			return x;
		if (ix > 0x7ff0000000000000)
			return __math_invalid(x);
		/* x is subnormal, normalize it.  */
		ix = asuint64(x * 0x1p52);
		top = ix >> 52;
		top -= 52;
	}

	/* argument reduction:
	   x = 4^e m; with integer e, and m in [1, 4)
	   m: fixed point representation [2.62]
	   2^e is the exponent part of the result.  */
	int even = top & 1;
	m = (ix << 11) | 0x8000000000000000;
	if (even) m >>= 1;
	top = (top + 0x3ff) >> 1;

	/* approximate r ~ 1/sqrt(m) and s ~ sqrt(m) when m in [1,4)

	   initial estimate:
	   7bit table lookup (1bit exponent and 6bit significand).

	   iterative approximation:
	   using 2 goldschmidt iterations with 32bit int arithmetics
	   and a final iteration with 64bit int arithmetics.

	   details:

	   the relative error (e = r0 sqrt(m)-1) of a linear estimate
	   (r0 = a m + b) is |e| < 0.085955 ~ 0x1.6p-4 at best,
	   a table lookup is faster and needs one less iteration
	   6 bit lookup table (128b) gives |e| < 0x1.f9p-8
	   7 bit lookup table (256b) gives |e| < 0x1.fdp-9
	   for single and double prec 6bit is enough but for quad
	   prec 7bit is needed (or modified iterations). to avoid
	   one more iteration >=13bit table would be needed (16k).

	   a newton-raphson iteration for r is
	     w = r*r
	     u = 3 - m*w
	     r = r*u/2
	   can use a goldschmidt iteration for s at the end or
	     s = m*r

	   first goldschmidt iteration is
	     s = m*r
	     u = 3 - s*r
	     r = r*u/2
	     s = s*u/2
	   next goldschmidt iteration is
	     u = 3 - s*r
	     r = r*u/2
	     s = s*u/2
	   and at the end r is not computed only s.

	   they use the same amount of operations and converge at the
	   same quadratic rate, i.e. if
	     r1 sqrt(m) - 1 = e, then
	     r2 sqrt(m) - 1 = -3/2 e^2 - 1/2 e^3
	   the advantage of goldschmidt is that the mul for s and r
	   are independent (computed in parallel), however it is not
	   "self synchronizing": it only uses the input m in the
	   first iteration so rounding errors accumulate. at the end
	   or when switching to larger precision arithmetics rounding
	   errors dominate so the first iteration should be used.

	   the fixed point representations are
	     m: 2.30 r: 0.32, s: 2.30, d: 2.30, u: 2.30, three: 2.30
	   and after switching to 64 bit
	     m: 2.62 r: 0.64, s: 2.62, d: 2.62, u: 2.62, three: 2.62  */

	static const uint64_t three = 0xc0000000;
	uint64_t r, s, d, u, i;

	i = (ix >> 46) % 128;
	r = (uint32_t)__rsqrt_tab[i] << 16;
	/* |r sqrt(m) - 1| < 0x1.fdp-9 */
	s = mul32(m>>32, r);
	/* |s/sqrt(m) - 1| < 0x1.fdp-9 */
	d = mul32(s, r);
	u = three - d;
	r = mul32(r, u) << 1;
	/* |r sqrt(m) - 1| < 0x1.7bp-16 */
	s = mul32(s, u) << 1;
	/* |s/sqrt(m) - 1| < 0x1.7bp-16 */
	d = mul32(s, r);
	u = three - d;
	r = mul32(r, u) << 1;
	/* |r sqrt(m) - 1| < 0x1.3704p-29 (measured worst-case) */
	r = r << 32;
	s = mul64(m, r);
	d = mul64(s, r);
	u = (three<<32) - d;
	s = mul64(s, u);  /* repr: 3.61 */
	/* -0x1p-57 < s - sqrt(m) < 0x1.8001p-61 */
	s = (s - 2) >> 9; /* repr: 12.52 */
	/* -0x1.09p-52 < s - sqrt(m) < -0x1.fffcp-63 */

	/* s < sqrt(m) < s + 0x1.09p-52,
	   compute nearest rounded result:
	   the nearest result to 52 bits is either s or s+0x1p-52,
	   we can decide by comparing (2^52 s + 0.5)^2 to 2^104 m.  */
	uint64_t d0, d1, d2;
	double y, t;
	d0 = (m << 42) - s*s;
	d1 = s - d0;
	d2 = d1 + s + 1;
	s += d1 >> 63;
	s &= 0x000fffffffffffff;
	s |= top << 52;
	y = asdouble(s);
	if (FENV_SUPPORT) {
		/* handle rounding modes and inexact exception:
		   only (s+1)^2 == 2^42 m case is exact otherwise
		   add a tiny value to cause the fenv effects.  */
		uint64_t tiny = UNLIKELY(d2==0) ? 0 : 0x0010000000000000;
		tiny |= (d1^d2) & 0x8000000000000000;
		t = asdouble(tiny);
		y = eval_as_double(y + t);
	}
	return y;

#endif /* __x86_64__ */
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(sqrt, sqrtl);
#endif
