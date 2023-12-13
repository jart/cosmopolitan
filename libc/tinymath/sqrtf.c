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

static inline uint32_t mul32(uint32_t a, uint32_t b)
{
	return (uint64_t)a*b >> 32;
}

/* see sqrt.c for more detailed comments.  */

/**
 * Returns square root of 𝑥.
 */
float sqrtf(float x)
{
#ifdef __SSE2__

	asm("sqrtss\t%1,%0" : "=x"(x) : "x"(x));
	return x;

#elif defined(__aarch64__)

	asm("fsqrt\t%s0,%s1" : "=w"(x) : "w"(x));
	return x;

#elif defined(__powerpc64__)

	asm("fsqrts\t%0,%1" : "=f"(x) : "f"(x));
	return x;

#elif defined(__riscv) && __riscv_flen >= 32

	asm("fsqrt.s\t%0,%1" : "=f"(x) : "f"(x));
	return x;

#elif defined(__s390x__) && (defined(__HTM__) || __ARCH__ >= 9)

	asm("sqebr\t%0,%1" : "=f"(x) : "f"(x));
	return x;

#else

	uint32_t ix, m, m1, m0, even, ey;

	ix = asuint(x);
	if (UNLIKELY(ix - 0x00800000 >= 0x7f800000 - 0x00800000)) {
		/* x < 0x1p-126 or inf or nan.  */
		if (ix * 2 == 0)
			return x;
		if (ix == 0x7f800000)
			return x;
		if (ix > 0x7f800000)
			return __math_invalidf(x);
		/* x is subnormal, normalize it.  */
		ix = asuint(x * 0x1p23f);
		ix -= 23 << 23;
	}

	/* x = 4^e m; with int e and m in [1, 4).  */
	even = ix & 0x00800000;
	m1 = (ix << 8) | 0x80000000;
	m0 = (ix << 7) & 0x7fffffff;
	m = even ? m0 : m1;

	/* 2^e is the exponent part of the return value.  */
	ey = ix >> 1;
	ey += 0x3f800000 >> 1;
	ey &= 0x7f800000;

	/* compute r ~ 1/sqrt(m), s ~ sqrt(m) with 2 goldschmidt iterations.  */
	static const uint32_t three = 0xc0000000;
	uint32_t r, s, d, u, i;
	i = (ix >> 17) % 128;
	r = (uint32_t)__rsqrt_tab[i] << 16;
	/* |r*sqrt(m) - 1| < 0x1p-8 */
	s = mul32(m, r);
	/* |s/sqrt(m) - 1| < 0x1p-8 */
	d = mul32(s, r);
	u = three - d;
	r = mul32(r, u) << 1;
	/* |r*sqrt(m) - 1| < 0x1.7bp-16 */
	s = mul32(s, u) << 1;
	/* |s/sqrt(m) - 1| < 0x1.7bp-16 */
	d = mul32(s, r);
	u = three - d;
	s = mul32(s, u);
	/* -0x1.03p-28 < s/sqrt(m) - 1 < 0x1.fp-31 */
	s = (s - 1)>>6;
	/* s < sqrt(m) < s + 0x1.08p-23 */

	/* compute nearest rounded result.  */
	uint32_t d0, d1, d2;
	float y, t;
	d0 = (m << 16) - s*s;
	d1 = s - d0;
	d2 = d1 + s + 1;
	s += d1 >> 31;
	s &= 0x007fffff;
	s |= ey;
	y = asfloat(s);
	if (FENV_SUPPORT) {
		/* handle rounding and inexact exception. */
		uint32_t tiny = UNLIKELY(d2==0) ? 0 : 0x01000000;
		tiny |= (d1^d2) & 0x80000000;
		t = asfloat(tiny);
		y = eval_as_float(y + t);
	}
	return y;

#endif /* __SSE2__ */
}
