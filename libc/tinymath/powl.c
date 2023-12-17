/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=8 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/errno.h"
#include "libc/math.h"
#include "libc/tinymath/internal.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

#ifdef __x86_64__

/**
 * Returns 𝑥^𝑦.
 * @note should take ~56ns
 */
long double powl(long double x, long double y) {
  long double t, u;
  if (!isunordered(x, y)) {
    if (!isinf(y)) {
      if (!isinf(x)) {
        if (x) {
          if (y) {
            if (x < 0 && y != truncl(y)) {
#ifndef __NO_MATH_ERRNO__
              errno = EDOM;
#endif
              return NAN;
            }
            asm("fyl2x" : "=t"(u) : "0"(fabsl(x)), "u"(y) : "st(1)");
            asm("fprem" : "=t"(t) : "0"(u), "u"(1.L));
            asm("f2xm1" : "=t"(t) : "0"(t));
            asm("fscale" : "=t"(t) : "0"(t + 1), "u"(u));
            if (signbit(x)) {
              if (y != truncl(y)) return -NAN;
              if ((int64_t)y & 1) t = -t;
            }
            return t;
          } else {
            return 1;
          }
        } else if (y > 0) {
          if (signbit(x) && y == truncl(y) && ((int64_t)y & 1)) {
            return -0.;
          } else {
            return 0;
          }
        } else if (!y) {
          return 1;
        } else {
#ifndef __NO_MATH_ERRNO__
          errno = ERANGE;
#endif
          if (y == truncl(y) && ((int64_t)y & 1)) {
            return copysignl(INFINITY, x);
          } else {
            return INFINITY;
          }
        }
      } else if (signbit(x)) {
        if (!y) return 1;
        x = y < 0 ? 0 : INFINITY;
        if (y == truncl(y) && ((int64_t)y & 1)) x = -x;
        return x;
      } else if (y < 0) {
        return 0;
      } else if (y > 0) {
        return INFINITY;
      } else {
        return 1;
      }
    } else {
      x = fabsl(x);
      if (x < 1) return signbit(y) ? INFINITY : 0;
      if (x > 1) return signbit(y) ? 0 : INFINITY;
      return 1;
    }
  } else if (!y || x == 1) {
    return 1;
  } else {
    return NAN;
  }
}

#elif LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384

asm(".ident\t\"\\n\\n\
OpenBSD libm (ISC License)\\n\
Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>\"");
asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* origin: OpenBSD /usr/src/lib/libm/src/ld80/e_powl.c */
/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*                                                      powl.c
 *
 *      Power function, long double precision
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, z, powl();
 *
 * z = powl( x, y );
 *
 *
 * DESCRIPTION:
 *
 * Computes x raised to the yth power.  Analytically,
 *
 *      x**y  =  exp( y log(x) ).
 *
 * Following Cody and Waite, this program uses a lookup table
 * of 2**-i/32 and pseudo extended precision arithmetic to
 * obtain several extra bits of accuracy in both the logarithm
 * and the exponential.
 *
 *
 * ACCURACY:
 *
 * The relative error of pow(x,y) can be estimated
 * by   y dl ln(2),   where dl is the absolute error of
 * the internally computed base 2 logarithm.  At the ends
 * of the approximation interval the logarithm equal 1/32
 * and its relative error is about 1 lsb = 1.1e-19.  Hence
 * the predicted relative error in the result is 2.3e-21 y .
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *
 *    IEEE     +-1000       40000      2.8e-18      3.7e-19
 * .001 < x < 1000, with log(x) uniformly distributed.
 * -1000 < y < 1000, y uniformly distributed.
 *
 *    IEEE     0,8700       60000      6.5e-18      1.0e-18
 * 0.99 < x < 1.01, 0 < y < 8700, uniformly distributed.
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * pow overflow     x**y > MAXNUM      INFINITY
 * pow underflow   x**y < 1/MAXNUM       0.0
 * pow domain      x<0 and y noninteger  0.0
 *
 */

/* Table size */
#define NXT 32

/* log(1+x) =  x - .5x^2 + x^3 *  P(z)/Q(z)
 * on the domain  2^(-1/32) - 1  <=  x  <=  2^(1/32) - 1
 */
static const long double P[] = {
 8.3319510773868690346226E-4L,
 4.9000050881978028599627E-1L,
 1.7500123722550302671919E0L,
 1.4000100839971580279335E0L,
};
static const long double Q[] = {
/* 1.0000000000000000000000E0L,*/
 5.2500282295834889175431E0L,
 8.4000598057587009834666E0L,
 4.2000302519914740834728E0L,
};
/* A[i] = 2^(-i/32), rounded to IEEE long double precision.
 * If i is even, A[i] + B[i/2] gives additional accuracy.
 */
static const long double A[33] = {
 1.0000000000000000000000E0L,
 9.7857206208770013448287E-1L,
 9.5760328069857364691013E-1L,
 9.3708381705514995065011E-1L,
 9.1700404320467123175367E-1L,
 8.9735453750155359320742E-1L,
 8.7812608018664974155474E-1L,
 8.5930964906123895780165E-1L,
 8.4089641525371454301892E-1L,
 8.2287773907698242225554E-1L,
 8.0524516597462715409607E-1L,
 7.8799042255394324325455E-1L,
 7.7110541270397041179298E-1L,
 7.5458221379671136985669E-1L,
 7.3841307296974965571198E-1L,
 7.2259040348852331001267E-1L,
 7.0710678118654752438189E-1L,
 6.9195494098191597746178E-1L,
 6.7712777346844636413344E-1L,
 6.6261832157987064729696E-1L,
 6.4841977732550483296079E-1L,
 6.3452547859586661129850E-1L,
 6.2092890603674202431705E-1L,
 6.0762367999023443907803E-1L,
 5.9460355750136053334378E-1L,
 5.8186242938878875689693E-1L,
 5.6939431737834582684856E-1L,
 5.5719337129794626814472E-1L,
 5.4525386633262882960438E-1L,
 5.3357020033841180906486E-1L,
 5.2213689121370692017331E-1L,
 5.1094857432705833910408E-1L,
 5.0000000000000000000000E-1L,
};
static const long double B[17] = {
 0.0000000000000000000000E0L,
 2.6176170809902549338711E-20L,
-1.0126791927256478897086E-20L,
 1.3438228172316276937655E-21L,
 1.2207982955417546912101E-20L,
-6.3084814358060867200133E-21L,
 1.3164426894366316434230E-20L,
-1.8527916071632873716786E-20L,
 1.8950325588932570796551E-20L,
 1.5564775779538780478155E-20L,
 6.0859793637556860974380E-21L,
-2.0208749253662532228949E-20L,
 1.4966292219224761844552E-20L,
 3.3540909728056476875639E-21L,
-8.6987564101742849540743E-22L,
-1.2327176863327626135542E-20L,
 0.0000000000000000000000E0L,
};

/* 2^x = 1 + x P(x),
 * on the interval -1/32 <= x <= 0
 */
static const long double R[] = {
 1.5089970579127659901157E-5L,
 1.5402715328927013076125E-4L,
 1.3333556028915671091390E-3L,
 9.6181291046036762031786E-3L,
 5.5504108664798463044015E-2L,
 2.4022650695910062854352E-1L,
 6.9314718055994530931447E-1L,
};

#define MEXP (NXT*16384.0L)
/* The following if denormal numbers are supported, else -MEXP: */
#define MNEXP (-NXT*(16384.0L+64.0L))
/* log2(e) - 1 */
#define LOG2EA 0.44269504088896340735992L

#define F W
#define Fa Wa
#define Fb Wb
#define G W
#define Ga Wa
#define Gb u
#define H W
#define Ha Wb
#define Hb Wb

static const long double MAXLOGL = 1.1356523406294143949492E4L;
static const long double MINLOGL = -1.13994985314888605586758E4L;
static const long double LOGE2L = 6.9314718055994530941723E-1L;
static const long double huge = 0x1p10000L;
/* XXX Prevent gcc from erroneously constant folding this. */
static const volatile long double twom10000 = 0x1p-10000L;

static long double reducl(long double);
static long double powil(long double, int);

long double powl(long double x, long double y)
{
	/* double F, Fa, Fb, G, Ga, Gb, H, Ha, Hb */
	int i, nflg, iyflg, yoddint;
	long e;
	volatile long double z=0;
	long double w=0, W=0, Wa=0, Wb=0, ya=0, yb=0, u=0;

	/* make sure no invalid exception is raised by nan comparision */
	if (isnan(x)) {
		if (!isnan(y) && y == 0.0)
			return 1.0;
		return x;
	}
	if (isnan(y)) {
		if (x == 1.0)
			return 1.0;
		return y;
	}
	if (x == 1.0)
		return 1.0; /* 1**y = 1, even if y is nan */
	if (x == -1.0 && !isfinite(y))
		return 1.0; /* -1**inf = 1 */
	if (y == 0.0)
		return 1.0; /* x**0 = 1, even if x is nan */
	if (y == 1.0)
		return x;
	if (y >= LDBL_MAX) {
		if (x > 1.0 || x < -1.0)
			return INFINITY;
		if (x != 0.0)
			return 0.0;
	}
	if (y <= -LDBL_MAX) {
		if (x > 1.0 || x < -1.0)
			return 0.0;
		if (x != 0.0 || y == -INFINITY)
			return INFINITY;
	}
	if (x >= LDBL_MAX) {
		if (y > 0.0)
			return INFINITY;
		return 0.0;
	}

	w = floorl(y);

	/* Set iyflg to 1 if y is an integer. */
	iyflg = 0;
	if (w == y)
		iyflg = 1;

	/* Test for odd integer y. */
	yoddint = 0;
	if (iyflg) {
		ya = fabsl(y);
		ya = floorl(0.5 * ya);
		yb = 0.5 * fabsl(w);
		if( ya != yb )
			yoddint = 1;
	}

	if (x <= -LDBL_MAX) {
		if (y > 0.0) {
			if (yoddint)
				return -INFINITY;
			return INFINITY;
		}
		if (y < 0.0) {
			if (yoddint)
				return -0.0;
			return 0.0;
		}
	}
	nflg = 0; /* (x<0)**(odd int) */
	if (x <= 0.0) {
		if (x == 0.0) {
			if (y < 0.0) {
				if (signbit(x) && yoddint)
					/* (-0.0)**(-odd int) = -inf, divbyzero */
					return -1.0/0.0;
				/* (+-0.0)**(negative) = inf, divbyzero */
				return 1.0/0.0;
			}
			if (signbit(x) && yoddint)
				return -0.0;
			return 0.0;
		}
		if (iyflg == 0)
			return (x - x) / (x - x); /* (x<0)**(non-int) is NaN */
		/* (x<0)**(integer) */
		if (yoddint)
			nflg = 1; /* negate result */
		x = -x;
	}
	/* (+integer)**(integer)  */
	if (iyflg && floorl(x) == x && fabsl(y) < 32768.0) {
		w = powil(x, (int)y);
		return nflg ? -w : w;
	}

	/* separate significand from exponent */
	x = frexpl(x, &i);
	e = i;

	/* find significand in antilog table A[] */
	i = 1;
	if (x <= A[17])
		i = 17;
	if (x <= A[i+8])
		i += 8;
	if (x <= A[i+4])
		i += 4;
	if (x <= A[i+2])
		i += 2;
	if (x >= A[1])
		i = -1;
	i += 1;

	/* Find (x - A[i])/A[i]
	 * in order to compute log(x/A[i]):
	 *
	 * log(x) = log( a x/a ) = log(a) + log(x/a)
	 *
	 * log(x/a) = log(1+v),  v = x/a - 1 = (x-a)/a
	 */
	x -= A[i];
	x -= B[i/2];
	x /= A[i];

	/* rational approximation for log(1+v):
	 *
	 * log(1+v)  =  v  -  v**2/2  +  v**3 P(v) / Q(v)
	 */
	z = x*x;
	w = x * (z * __polevll(x, P, 3) / __p1evll(x, Q, 3));
	w = w - 0.5*z;

	/* Convert to base 2 logarithm:
	 * multiply by log2(e) = 1 + LOG2EA
	 */
	z = LOG2EA * w;
	z += w;
	z += LOG2EA * x;
	z += x;

	/* Compute exponent term of the base 2 logarithm. */
	w = -i;
	w /= NXT;
	w += e;
	/* Now base 2 log of x is w + z. */

	/* Multiply base 2 log by y, in extended precision. */

	/* separate y into large part ya
	 * and small part yb less than 1/NXT
	 */
	ya = reducl(y);
	yb = y - ya;

	/* (w+z)(ya+yb)
	 * = w*ya + w*yb + z*y
	 */
	F = z * y  +  w * yb;
	Fa = reducl(F);
	Fb = F - Fa;

	G = Fa + w * ya;
	Ga = reducl(G);
	Gb = G - Ga;

	H = Fb + Gb;
	Ha = reducl(H);
	w = (Ga + Ha) * NXT;

	/* Test the power of 2 for overflow */
	if (w > MEXP)
		return huge * huge;  /* overflow */
	if (w < MNEXP)
		return twom10000 * twom10000;  /* underflow */

	e = w;
	Hb = H - Ha;

	if (Hb > 0.0) {
		e += 1;
		Hb -= 1.0/NXT;  /*0.0625L;*/
	}

	/* Now the product y * log2(x)  =  Hb + e/NXT.
	 *
	 * Compute base 2 exponential of Hb,
	 * where -0.0625 <= Hb <= 0.
	 */
	z = Hb * __polevll(Hb, R, 6);  /*  z = 2**Hb - 1  */

	/* Express e/NXT as an integer plus a negative number of (1/NXT)ths.
	 * Find lookup table entry for the fractional power of 2.
	 */
	if (e < 0)
		i = 0;
	else
		i = 1;
	i = e/NXT + i;
	e = NXT*i - e;
	w = A[e];
	z = w * z;  /*  2**-e * ( 1 + (2**Hb-1) )  */
	z = z + w;
	z = scalbnl(z, i);  /* multiply by integer power of 2 */

	if (nflg)
		z = -z;
	return z;
}


/* Find a multiple of 1/NXT that is within 1/NXT of x. */
static long double reducl(long double x)
{
	long double t;

	t = x * NXT;
	t = floorl(t);
	t = t / NXT;
	return t;
}

/*
 *      Positive real raised to integer power, long double precision
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, powil();
 * int n;
 *
 * y = powil( x, n );
 *
 *
 * DESCRIPTION:
 *
 * Returns argument x>0 raised to the nth power.
 * The routine efficiently decomposes n as a sum of powers of
 * two. The desired power is a product of two-to-the-kth
 * powers of x.  Thus to compute the 32767 power of x requires
 * 28 multiplications instead of 32767 multiplications.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   x domain   n domain  # trials      peak         rms
 *    IEEE     .001,1000  -1022,1023  50000       4.3e-17     7.8e-18
 *    IEEE        1,2     -1022,1023  20000       3.9e-17     7.6e-18
 *    IEEE     .99,1.01     0,8700    10000       3.6e-16     7.2e-17
 *
 * Returns MAXNUM on overflow, zero on underflow.
 */

static long double powil(long double x, int nn)
{
	long double ww, y;
	long double s;
	int n, e, sign, lx;

	if (nn == 0)
		return 1.0;

	if (nn < 0) {
		sign = -1;
		n = -nn;
	} else {
		sign = 1;
		n = nn;
	}

	/* Overflow detection */

	/* Calculate approximate logarithm of answer */
	s = x;
	s = frexpl( s, &lx);
	e = (lx - 1)*n;
	if ((e == 0) || (e > 64) || (e < -64)) {
		s = (s - 7.0710678118654752e-1L) / (s +  7.0710678118654752e-1L);
		s = (2.9142135623730950L * s - 0.5 + lx) * nn * LOGE2L;
	} else {
		s = LOGE2L * e;
	}

	if (s > MAXLOGL)
		return huge * huge;  /* overflow */

	if (s < MINLOGL)
		return twom10000 * twom10000;  /* underflow */
	/* Handle tiny denormal answer, but with less accuracy
	 * since roundoff error in 1.0/x will be amplified.
	 * The precise demarcation should be the gradual underflow threshold.
	 */
	if (s < -MAXLOGL+2.0) {
		x = 1.0/x;
		sign = -sign;
	}

	/* First bit of the power */
	if (n & 1)
		y = x;
	else
		y = 1.0;

	ww = x;
	n >>= 1;
	while (n) {
		ww = ww * ww;   /* arg to the 2-to-the-kth power */
		if (n & 1)     /* if that bit is set, then include in product */
			y *= ww;
		n >>= 1;
	}

	if (sign < 0)
		y = 1.0/y;
	return y;
}

#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
#include "libc/tinymath/freebsd.internal.h"

asm(".ident\t\"\\n\\n\
OpenBSD libm (ISC License)\\n\
Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>\"");

/*-
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

/*
 * Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* powl(x,y) return x**y
 *
 *		      n
 * Method:  Let x =  2   * (1+f)
 *	1. Compute and return log2(x) in two pieces:
 *		log2(x) = w1 + w2,
 *	   where w1 has 113-53 = 60 bit trailing zeros.
 *	2. Perform y*log2(x) = n+y' by simulating multi-precision
 *	   arithmetic, where |y'|<=0.5.
 *	3. Return x**y = 2**n*exp(y'*log2)
 *
 * Special cases:
 *	1.  (anything) ** 0  is 1
 *	2.  (anything) ** 1  is itself
 *	3.  (anything) ** NAN is NAN
 *	4.  NAN ** (anything except 0) is NAN
 *	5.  +-(|x| > 1) **  +INF is +INF
 *	6.  +-(|x| > 1) **  -INF is +0
 *	7.  +-(|x| < 1) **  +INF is +0
 *	8.  +-(|x| < 1) **  -INF is +INF
 *	9.  +-1         ** +-INF is NAN
 *	10. +0 ** (+anything except 0, NAN)               is +0
 *	11. -0 ** (+anything except 0, NAN, odd integer)  is +0
 *	12. +0 ** (-anything except 0, NAN)               is +INF
 *	13. -0 ** (-anything except 0, NAN, odd integer)  is +INF
 *	14. -0 ** (odd integer) = -( +0 ** (odd integer) )
 *	15. +INF ** (+anything except 0,NAN) is +INF
 *	16. +INF ** (-anything except 0,NAN) is +0
 *	17. -INF ** (anything)  = -0 ** (-anything)
 *	18. (-anything) ** (integer) is (-1)**(integer)*(+anything**integer)
 *	19. (-anything except 0 and inf) ** (non-integer) is NAN
 *
 */

static const long double bp[] = {
  1.0L,
  1.5L,
};

/* log_2(1.5) */
static const long double dp_h[] = {
  0.0,
  5.8496250072115607565592654282227158546448E-1L
};

/* Low part of log_2(1.5) */
static const long double dp_l[] = {
  0.0,
  1.0579781240112554492329533686862998106046E-16L
};

static const long double zero = 0.0L,
  one = 1.0L,
  two = 2.0L,
  two113 = 1.0384593717069655257060992658440192E34L,
  huge = 1.0e3000L,
  tiny = 1.0e-3000L;

/* 3/2 log x = 3 z + z^3 + z^3 (z^2 R(z^2))
   z = (x-1)/(x+1)
   1 <= x <= 1.25
   Peak relative error 2.3e-37 */
static const long double LN[] =
{
 -3.0779177200290054398792536829702930623200E1L,
  6.5135778082209159921251824580292116201640E1L,
 -4.6312921812152436921591152809994014413540E1L,
  1.2510208195629420304615674658258363295208E1L,
 -9.9266909031921425609179910128531667336670E-1L
};
static const long double LD[] =
{
 -5.129862866715009066465422805058933131960E1L,
  1.452015077564081884387441590064272782044E2L,
 -1.524043275549860505277434040464085593165E2L,
  7.236063513651544224319663428634139768808E1L,
 -1.494198912340228235853027849917095580053E1L
  /* 1.0E0 */
};

/* exp(x) = 1 + x - x / (1 - 2 / (x - x^2 R(x^2)))
   0 <= x <= 0.5
   Peak relative error 5.7e-38  */
static const long double PN[] =
{
  5.081801691915377692446852383385968225675E8L,
  9.360895299872484512023336636427675327355E6L,
  4.213701282274196030811629773097579432957E4L,
  5.201006511142748908655720086041570288182E1L,
  9.088368420359444263703202925095675982530E-3L,
};
static const long double PD[] =
{
  3.049081015149226615468111430031590411682E9L,
  1.069833887183886839966085436512368982758E8L,
  8.259257717868875207333991924545445705394E5L,
  1.872583833284143212651746812884298360922E3L,
  /* 1.0E0 */
};

static const long double
  /* ln 2 */
  lg2 = 6.9314718055994530941723212145817656807550E-1L,
  lg2_h = 6.9314718055994528622676398299518041312695E-1L,
  lg2_l = 2.3190468138462996154948554638754786504121E-17L,
  ovt = 8.0085662595372944372e-0017L,
  /* 2/(3*log(2)) */
  cp = 9.6179669392597560490661645400126142495110E-1L,
  cp_h = 9.6179669392597555432899980587535537779331E-1L,
  cp_l = 5.0577616648125906047157785230014751039424E-17L;

long double
powl(long double x, long double y)
{
  long double z, ax, z_h, z_l, p_h, p_l;
  long double yy1, t1, t2, r, s, t, u, v, w;
  long double s2, s_h, s_l, t_h, t_l;
  int32_t i, j, k, yisint, n;
  uint32_t ix, iy;
  int32_t hx, hy;
  ieee_quad_shape_type o, p, q;

  p.value = x;
  hx = p.parts32.mswhi;
  ix = hx & 0x7fffffff;

  q.value = y;
  hy = q.parts32.mswhi;
  iy = hy & 0x7fffffff;


  /* y==zero: x**0 = 1 */
  if ((iy | q.parts32.mswlo | q.parts32.lswhi | q.parts32.lswlo) == 0)
    return one;

  /* 1.0**y = 1; -1.0**+-Inf = 1 */
  if (x == one)
    return one;
  if (x == -1.0L && iy == 0x7fff0000
      && (q.parts32.mswlo | q.parts32.lswhi | q.parts32.lswlo) == 0)
    return one;

  /* +-NaN return x+y */
  if ((ix > 0x7fff0000)
      || ((ix == 0x7fff0000)
	  && ((p.parts32.mswlo | p.parts32.lswhi | p.parts32.lswlo) != 0))
      || (iy > 0x7fff0000)
      || ((iy == 0x7fff0000)
	  && ((q.parts32.mswlo | q.parts32.lswhi | q.parts32.lswlo) != 0)))
    return nan_mix(x, y);

  /* determine if y is an odd int when x < 0
   * yisint = 0       ... y is not an integer
   * yisint = 1       ... y is an odd int
   * yisint = 2       ... y is an even int
   */
  yisint = 0;
  if (hx < 0)
    {
      if (iy >= 0x40700000)	/* 2^113 */
	yisint = 2;		/* even integer y */
      else if (iy >= 0x3fff0000)	/* 1.0 */
	{
	  if (floorl (y) == y)
	    {
	      z = 0.5 * y;
	      if (floorl (z) == z)
		yisint = 2;
	      else
		yisint = 1;
	    }
	}
    }

  /* special value of y */
  if ((q.parts32.mswlo | q.parts32.lswhi | q.parts32.lswlo) == 0)
    {
      if (iy == 0x7fff0000)	/* y is +-inf */
	{
	  if (((ix - 0x3fff0000) | p.parts32.mswlo | p.parts32.lswhi |
	    p.parts32.lswlo) == 0)
	    return y - y;	/* +-1**inf is NaN */
	  else if (ix >= 0x3fff0000)	/* (|x|>1)**+-inf = inf,0 */
	    return (hy >= 0) ? y : zero;
	  else			/* (|x|<1)**-,+inf = inf,0 */
	    return (hy < 0) ? -y : zero;
	}
      if (iy == 0x3fff0000)
	{			/* y is  +-1 */
	  if (hy < 0)
	    return one / x;
	  else
	    return x;
	}
      if (hy == 0x40000000)
	return x * x;		/* y is  2 */
      if (hy == 0x3ffe0000)
	{			/* y is  0.5 */
	  if (hx >= 0)		/* x >= +0 */
	    return sqrtl (x);
	}
    }

  ax = fabsl (x);
  /* special value of x */
  if ((p.parts32.mswlo | p.parts32.lswhi | p.parts32.lswlo) == 0)
    {
      if (ix == 0x7fff0000 || ix == 0 || ix == 0x3fff0000)
	{
	  z = ax;		/*x is +-0,+-inf,+-1 */
	  if (hy < 0)
	    z = one / z;	/* z = (1/|x|) */
	  if (hx < 0)
	    {
	      if (((ix - 0x3fff0000) | yisint) == 0)
		{
		  z = (z - z) / (z - z);	/* (-1)**non-int is NaN */
		}
	      else if (yisint == 1)
		z = -z;		/* (x<0)**odd = -(|x|**odd) */
	    }
	  return z;
	}
    }

  /* (x<0)**(non-int) is NaN */
  if (((((uint32_t) hx >> 31) - 1) | yisint) == 0)
    return (x - x) / (x - x);

  /* |y| is huge.
     2^-16495 = 1/2 of smallest representable value.
     If (1 - 1/131072)^y underflows, y > 1.4986e9 */
  if (iy > 0x401d654b)
    {
      /* if (1 - 2^-113)^y underflows, y > 1.1873e38 */
      if (iy > 0x407d654b)
	{
	  if (ix <= 0x3ffeffff)
	    return (hy < 0) ? huge * huge : tiny * tiny;
	  if (ix >= 0x3fff0000)
	    return (hy > 0) ? huge * huge : tiny * tiny;
	}
      /* over/underflow if x is not close to one */
      if (ix < 0x3ffeffff)
	return (hy < 0) ? huge * huge : tiny * tiny;
      if (ix > 0x3fff0000)
	return (hy > 0) ? huge * huge : tiny * tiny;
    }

  n = 0;
  /* take care subnormal number */
  if (ix < 0x00010000)
    {
      ax *= two113;
      n -= 113;
      o.value = ax;
      ix = o.parts32.mswhi;
    }
  n += ((ix) >> 16) - 0x3fff;
  j = ix & 0x0000ffff;
  /* determine interval */
  ix = j | 0x3fff0000;		/* normalize ix */
  if (j <= 0x3988)
    k = 0;			/* |x|<sqrt(3/2) */
  else if (j < 0xbb67)
    k = 1;			/* |x|<sqrt(3)   */
  else
    {
      k = 0;
      n += 1;
      ix -= 0x00010000;
    }

  o.value = ax;
  o.parts32.mswhi = ix;
  ax = o.value;

  /* compute s = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
  u = ax - bp[k];		/* bp[0]=1.0, bp[1]=1.5 */
  v = one / (ax + bp[k]);
  s = u * v;
  s_h = s;

  o.value = s_h;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  s_h = o.value;
  /* t_h=ax+bp[k] High */
  t_h = ax + bp[k];
  o.value = t_h;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  t_h = o.value;
  t_l = ax - (t_h - bp[k]);
  s_l = v * ((u - s_h * t_h) - s_h * t_l);
  /* compute log(ax) */
  s2 = s * s;
  u = LN[0] + s2 * (LN[1] + s2 * (LN[2] + s2 * (LN[3] + s2 * LN[4])));
  v = LD[0] + s2 * (LD[1] + s2 * (LD[2] + s2 * (LD[3] + s2 * (LD[4] + s2))));
  r = s2 * s2 * u / v;
  r += s_l * (s_h + s);
  s2 = s_h * s_h;
  t_h = 3.0 + s2 + r;
  o.value = t_h;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  t_h = o.value;
  t_l = r - ((t_h - 3.0) - s2);
  /* u+v = s*(1+...) */
  u = s_h * t_h;
  v = s_l * t_h + t_l * s;
  /* 2/(3log2)*(s+...) */
  p_h = u + v;
  o.value = p_h;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  p_h = o.value;
  p_l = v - (p_h - u);
  z_h = cp_h * p_h;		/* cp_h+cp_l = 2/(3*log2) */
  z_l = cp_l * p_h + p_l * cp + dp_l[k];
  /* log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l */
  t = (long double) n;
  t1 = (((z_h + z_l) + dp_h[k]) + t);
  o.value = t1;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  t1 = o.value;
  t2 = z_l - (((t1 - t) - dp_h[k]) - z_h);

  /* s (sign of result -ve**odd) = -1 else = 1 */
  s = one;
  if (((((uint32_t) hx >> 31) - 1) | (yisint - 1)) == 0)
    s = -one;			/* (-ve)**(odd int) */

  /* split up y into yy1+y2 and compute (yy1+y2)*(t1+t2) */
  yy1 = y;
  o.value = yy1;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  yy1 = o.value;
  p_l = (y - yy1) * t1 + y * t2;
  p_h = yy1 * t1;
  z = p_l + p_h;
  o.value = z;
  j = o.parts32.mswhi;
  if (j >= 0x400d0000) /* z >= 16384 */
    {
      /* if z > 16384 */
      if (((j - 0x400d0000) | o.parts32.mswlo | o.parts32.lswhi |
	o.parts32.lswlo) != 0)
	return s * huge * huge;	/* overflow */
      else
	{
	  if (p_l + ovt > z - p_h)
	    return s * huge * huge;	/* overflow */
	}
    }
  else if ((j & 0x7fffffff) >= 0x400d01b9)	/* z <= -16495 */
    {
      /* z < -16495 */
      if (((j - 0xc00d01bc) | o.parts32.mswlo | o.parts32.lswhi |
	o.parts32.lswlo)
	  != 0)
	return s * tiny * tiny;	/* underflow */
      else
	{
	  if (p_l <= z - p_h)
	    return s * tiny * tiny;	/* underflow */
	}
    }
  /* compute 2**(p_h+p_l) */
  i = j & 0x7fffffff;
  k = (i >> 16) - 0x3fff;
  n = 0;
  if (i > 0x3ffe0000)
    {				/* if |z| > 0.5, set n = [z+0.5] */
      n = floorl (z + 0.5L);
      t = n;
      p_h -= t;
    }
  t = p_l + p_h;
  o.value = t;
  o.parts32.lswlo = 0;
  o.parts32.lswhi &= 0xf8000000;
  t = o.value;
  u = t * lg2_h;
  v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
  z = u + v;
  w = v - (z - u);
  /*  exp(z) */
  t = z * z;
  u = PN[0] + t * (PN[1] + t * (PN[2] + t * (PN[3] + t * PN[4])));
  v = PD[0] + t * (PD[1] + t * (PD[2] + t * (PD[3] + t)));
  t1 = z - t * u / v;
  r = (z * t1) / (t1 - two) - (w + z * w);
  z = one - (r - z);
  o.value = z;
  j = o.parts32.mswhi;
  j += (n << 16);
  if ((j >> 16) <= 0)
    z = scalbnl (z, n);	/* subnormal output */
  else
    {
      o.parts32.mswhi = j;
      z = o.value;
    }
  return s * z;
}

#endif /* __x86_64__ */

__weak_reference(powl, __powl_finite);

#endif /* long double is long */
