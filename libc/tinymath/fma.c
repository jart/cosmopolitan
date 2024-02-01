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
#include "libc/nexgen32e/x86feature.h"

asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

#define ASUINT64(x) ((union {double f; uint64_t i;}){x}).i
#define ZEROINFNAN (0x7ff-0x3ff-52-1)

static inline int a_clz_64(uint64_t x)
{
#ifdef __GNUC__
	if (!x) return 63;
	return __builtin_clzll(x);
#else
	uint32_t y;
	int r;
	if (x>>32) y=x>>32, r=0; else y=x, r=32;
	if (y>>16) y>>=16; else r |= 16;
	if (y>>8) y>>=8; else r |= 8;
	if (y>>4) y>>=4; else r |= 4;
	if (y>>2) y>>=2; else r |= 2;
	return r | !(y>>1);
#endif
}

struct num { uint64_t m; int e; int sign; };

static struct num normalize(double x)
{
	uint64_t ix = ASUINT64(x);
	int e = ix>>52;
	int sign = e & 0x800;
	e &= 0x7ff;
	if (!e) {
		ix = ASUINT64(x*0x1p63);
		e = ix>>52 & 0x7ff;
		e = e ? e-63 : 0x800;
	}
	ix &= (1ull<<52)-1;
	ix |= 1ull<<52;
	ix <<= 1;
	e -= 0x3ff + 52 + 1;
	return (struct num){ix,e,sign};
}

static void mul(uint64_t *hi, uint64_t *lo, uint64_t x, uint64_t y)
{
	uint64_t t1,t2,t3;
	uint64_t xlo = (uint32_t)x, xhi = x>>32;
	uint64_t ylo = (uint32_t)y, yhi = y>>32;
	t1 = xlo*ylo;
	t2 = xlo*yhi + xhi*ylo;
	t3 = xhi*yhi;
	*lo = t1 + (t2<<32);
	*hi = t3 + (t2>>32) + (t1 > *lo);
}

/**
 * Performs fused multiply add.
 *
 * @return `𝑥 * 𝑦 + 𝑧` rounded as one ternary operation
 */
double fma(double x, double y, double z)
{
#if defined(__x86_64__) && defined(__FMA__)

	// Intel Haswell+ (c. 2013)
	// AMD Piledriver+ (c. 2011)
	asm("vfmadd132sd\t%1,%2,%0" : "+x"(x) : "x"(y), "x"(z));
	return x;

#elif defined(__x86_64__) && defined(__FMA4__)

	// AMD Bulldozer+ (c. 2011)
	asm("vfmaddsd\t%3,%2,%1,%0" : "=x"(x) : "x"(x), "x"(y), "x"(z));
	return x;

#elif defined(__aarch64__)

	asm("fmadd\t%d0,%d1,%d2,%d3" : "=w"(x) : "w"(x), "w"(y), "w"(z));
	return x;

#elif defined(__powerpc64__)

	asm("fmadd\t%0,%1,%2,%3" : "=d"(x) : "d"(x), "d"(y), "d"(z));
	return x;

#elif defined(__riscv) && __riscv_flen >= 64

	asm("fmadd.d\t%0,%1,%2,%3" : "=f"(x) : "f"(x), "f"(y), "f"(z));
	return x;

#elif defined(__s390x__)

	asm("madbr\t%0,\t%1,\t%2" : "+f"(z) : "f"(x), "f"(y));
	return z;

#else
/* #pragma STDC FENV_ACCESS ON */

#ifdef __x86_64__
	if (X86_HAVE(FMA)) {
		asm("vfmadd132sd\t%1,%2,%0" : "+x"(x) : "x"(y), "x"(z));
		return x;
	} else if (X86_HAVE(FMA4)) {
		asm("vfmaddsd\t%3,%2,%1,%0" : "=x"(x) : "x"(x), "x"(y), "x"(z));
		return x;
	}
#endif

	/* normalize so top 10bits and last bit are 0 */
	struct num nx, ny, nz;
	nx = normalize(x);
	ny = normalize(y);
	nz = normalize(z);

	if (nx.e >= ZEROINFNAN || ny.e >= ZEROINFNAN)
		return x*y + z;
	if (nz.e >= ZEROINFNAN) {
		if (nz.e > ZEROINFNAN) /* z==0 */
			return x*y + z;
		return z;
	}

	/* mul: r = x*y */
	uint64_t rhi, rlo, zhi, zlo;
	mul(&rhi, &rlo, nx.m, ny.m);
	/* either top 20 or 21 bits of rhi and last 2 bits of rlo are 0 */

	/* align exponents */
	int e = nx.e + ny.e;
	int d = nz.e - e;
	/* shift bits z<<=kz, r>>=kr, so kz+kr == d, set e = e+kr (== ez-kz) */
	if (d > 0) {
		if (d < 64) {
			zlo = nz.m<<d;
			zhi = nz.m>>(64-d);
		} else {
			zlo = 0;
			zhi = nz.m;
			e = nz.e - 64;
			d -= 64;
			if (d == 0) {
			} else if (d < 64) {
				rlo = rhi<<(64-d) | rlo>>d | !!(rlo<<(64-d));
				rhi = rhi>>d;
			} else {
				rlo = 1;
				rhi = 0;
			}
		}
	} else {
		zhi = 0;
		d = -d;
		if (d == 0) {
			zlo = nz.m;
		} else if (d < 64) {
			zlo = nz.m>>d | !!(nz.m<<(64-d));
		} else {
			zlo = 1;
		}
	}

	/* add */
	int sign = nx.sign^ny.sign;
	int samesign = !(sign^nz.sign);
	int nonzero = 1;
	if (samesign) {
		/* r += z */
		rlo += zlo;
		rhi += zhi + (rlo < zlo);
	} else {
		/* r -= z */
		uint64_t t = rlo;
		rlo -= zlo;
		rhi = rhi - zhi - (t < rlo);
		if (rhi>>63) {
			rlo = -rlo;
			rhi = -rhi-!!rlo;
			sign = !sign;
		}
		nonzero = !!rhi;
	}

	/* set rhi to top 63bit of the result (last bit is sticky) */
	if (nonzero) {
		e += 64;
		d = a_clz_64(rhi)-1;
		/* note: d > 0 */
		rhi = rhi<<d | rlo>>(64-d) | !!(rlo<<d);
	} else if (rlo) {
		d = a_clz_64(rlo)-1;
		if (d < 0)
			rhi = rlo>>1 | (rlo&1);
		else
			rhi = rlo<<d;
	} else {
		/* exact +-0 */
		return x*y + z;
	}
	e -= d;

	/* convert to double */
	int64_t i = rhi; /* i is in [1<<62,(1<<63)-1] */
	if (sign)
		i = -i;
	double r = i; /* |r| is in [0x1p62,0x1p63] */

	if (e < -1022-62) {
		/* result is subnormal before rounding */
		if (e == -1022-63) {
			double c = 0x1p63;
			if (sign)
				c = -c;
			if (r == c) {
				/* min normal after rounding, underflow depends
				   on arch behaviour which can be imitated by
				   a double to float conversion */
				float fltmin = 0x0.ffffff8p-63*FLT_MIN * r;
				return DBL_MIN/FLT_MIN * fltmin;
			}
			/* one bit is lost when scaled, add another top bit to
			   only round once at conversion if it is inexact */
			if (rhi << 53) {
				i = rhi>>1 | (rhi&1) | 1ull<<62;
				if (sign)
					i = -i;
				r = i;
				r = 2*r - c; /* remove top bit */

				/* raise underflow portably, such that it
				   cannot be optimized away */
				{
					double_t tiny = DBL_MIN/FLT_MIN * r;
					r += (double)(tiny*tiny) * (r-r);
				}
			}
		} else {
			/* only round once when scaled */
			d = 10;
			i = ( rhi>>d | !!(rhi<<(64-d)) ) << d;
			if (sign)
				i = -i;
			r = i;
		}
	}
	return scalbn(r, e);

#endif /* __x86_64__ */
}

#if LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024
__weak_reference(fma, fmal);
#endif
