/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set noet ft=c ts=2 sts=2 sw=2 fenc=utf-8                             :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"

#if LDBL_MANT_DIG == 64 && LDBL_MAX_EXP == 16384
#include "libc/tinymath/internal.h"

asm(".ident\t\"\\n\\n\
OpenBSD libm (ISC License)\\n\
Copyright (c) 2008 Stephen L. Moshier <steve@moshier.net>\"");
asm(".ident\t\"\\n\\n\
Musl libc (MIT License)\\n\
Copyright 2005-2014 Rich Felker, et. al.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* origin: OpenBSD /usr/src/lib/libm/src/ld80/e_expl.c */
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

/*
 *      Exponential function, long double precision
 *
 *
 * SYNOPSIS:
 *
 * long double x, y, expl();
 *
 * y = expl( x );
 *
 *
 * DESCRIPTION:
 *
 * Returns e (2.71828...) raised to the x power.
 *
 * Range reduction is accomplished by separating the argument
 * into an integer k and fraction f such that
 *
 *     x    k  f
 *    e  = 2  e.
 *
 * A Pade' form of degree 5/6 is used to approximate exp(f) - 1
 * in the basic range [-0.5 ln 2, 0.5 ln 2].
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      +-10000     50000       1.12e-19    2.81e-20
 *
 *
 * Error amplification in the exponential function can be
 * a serious matter.  The error propagation involves
 * exp( X(1+delta) ) = exp(X) ( 1 + X*delta + ... ),
 * which shows that a 1 lsb error in representing X produces
 * a relative error of X times 1 lsb in the function.
 * While the routine gives an accurate result for arguments
 * that are exactly represented by a long double precision
 * computer number, the result contains amplified roundoff
 * error for large arguments not exactly represented.
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * exp underflow    x < MINLOG         0.0
 * exp overflow     x > MAXLOG         MAXNUM
 *
 */

static const long double P[3] = {
 1.2617719307481059087798E-4L,
 3.0299440770744196129956E-2L,
 9.9999999999999999991025E-1L,
};
static const long double Q[4] = {
 3.0019850513866445504159E-6L,
 2.5244834034968410419224E-3L,
 2.2726554820815502876593E-1L,
 2.0000000000000000000897E0L,
};
static const long double
LN2HI = 6.9314575195312500000000E-1L,
LN2LO = 1.4286068203094172321215E-6L,
LOG2E = 1.4426950408889634073599E0L;

/**
 * Returns 𝑒ˣ.
 */
long double expl(long double x)
{
	long double px, xx;
	int k;

	if (isnan(x))
		return x;
	if (x > 11356.5234062941439488L) /* x > ln(2^16384 - 0.5) */
		return x * 0x1p16383L;
	if (x < -11399.4985314888605581L) /* x < ln(2^-16446) */
		return -0x1p-16445L/x;

	/* Express e**x = e**f 2**k
	 *   = e**(f + k ln(2))
	 */
	px = floorl(LOG2E * x + 0.5);
	k = px;
	x -= px * LN2HI;
	x -= px * LN2LO;

	/* rational approximation of the fractional part:
	 * e**x =  1 + 2x P(x**2)/(Q(x**2) - x P(x**2))
	 */
	xx = x * x;
	px = x * __polevll(xx, P, 2);
	x = px/(__polevll(xx, Q, 3) - px);
	x = 1.0 + 2.0 * x;
	return scalbnl(x, k);
}

#elif LDBL_MANT_DIG == 113 && LDBL_MAX_EXP == 16384
#include "libc/tinymath/freebsd.internal.h"

asm(".ident\t\"\\n\\n\
FreeBSD libm (BSD-2 License)\\n\
Copyright (c) 2005-2011, Bruce D. Evans, Steven G. Kargl, David Schultz.\"");
asm(".ident\t\"\\n\\n\
fdlibm (fdlibm license)\\n\
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2009-2013 Steven G. Kargl
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Optimized by Bruce D. Evans.
 */

/*
 * ld128 version of s_expl.c.  See ../ld80/s_expl.c for most comments.
 */

/* XXX Prevent compilers from erroneously constant folding these: */
static const volatile long double
huge = 0x1p10000L,
tiny = 0x1p-10000L;

static const long double
twom10000 = 0x1p-10000L;

static const long double
/* log(2**16384 - 0.5) rounded towards zero: */
/* log(2**16384 - 0.5 + 1) rounded towards zero for expm1l() is the same: */
o_threshold =  11356.523406294143949491931077970763428L,
/* log(2**(-16381-64-1)) rounded towards zero: */
u_threshold = -11433.462743336297878837243843452621503L;

static const double
/*
 * ln2/INTERVALS = L1+L2 (hi+lo decomposition for multiplication).  L1 must
 * have at least 22 (= log2(|LDBL_MIN_EXP-extras|) + log2(INTERVALS)) lowest
 * bits zero so that multiplication of it by n is exact.
 */
INV_L = 1.8466496523378731e+2,		/*  0x171547652b82fe.0p-45 */
L2 = -1.0253670638894731e-29;		/* -0x1.9ff0342542fc3p-97 */
static const long double
/* 0x1.62e42fefa39ef35793c768000000p-8 */
L1 =  5.41521234812457272982212595914567508e-3L;

/*
 * XXX values in hex in comments have been lost (or were never present)
 * from here.
 */
static const long double
/*
 * Domain [-0.002708, 0.002708], range ~[-2.4021e-38, 2.4234e-38]:
 * |exp(x) - p(x)| < 2**-124.9
 * (0.002708 is ln2/(2*INTERVALS) rounded up a little).
 *
 * XXX the coeffs aren't very carefully rounded, and I get 3.6 more bits.
 */
A2  =  0.5,
A3  =  1.66666666666666666666666666651085500e-1L,
A4  =  4.16666666666666666666666666425885320e-2L,
A5  =  8.33333333333333333334522877160175842e-3L,
A6  =  1.38888888888888888889971139751596836e-3L;

static const double
A7  =  1.9841269841269470e-4,		/*  0x1.a01a01a019f91p-13 */
A8  =  2.4801587301585286e-5,		/*  0x1.71de3ec75a967p-19 */
A9  =  2.7557324277411235e-6,		/*  0x1.71de3ec75a967p-19 */
A10 =  2.7557333722375069e-7;		/*  0x1.27e505ab56259p-22 */

/**
 * Returns 𝑒ˣ.
 */
long double
expl(long double x)
{
	union IEEEl2bits u;
	long double hi, lo, t, twopk;
	int k;
	uint16_t hx, ix;

	DOPRINT_START(&x);

	/* Filter out exceptional cases. */
	u.e = x;
	hx = u.xbits.expsign;
	ix = hx & 0x7fff;
	if (ix >= BIAS + 13) {		/* |x| >= 8192 or x is NaN */
		if (ix == BIAS + LDBL_MAX_EXP) {
			if (hx & 0x8000)  /* x is -Inf or -NaN */
				RETURNP(-1 / x);
			RETURNP(x + x);	/* x is +Inf or +NaN */
		}
		if (x > o_threshold)
			RETURNP(huge * huge);
		if (x < u_threshold)
			RETURNP(tiny * tiny);
	} else if (ix < BIAS - 114) {	/* |x| < 0x1p-114 */
		RETURN2P(1, x);		/* 1 with inexact iff x != 0 */
	}

	ENTERI();

	twopk = 1;
	__k_expl(x, &hi, &lo, &k);
	t = SUM2P(hi, lo);

	/* Scale by 2**k. */
	/*
	 * XXX sparc64 multiplication was so slow that scalbnl() is faster,
	 * but performance on aarch64 and riscv hasn't yet been quantified.
	 */
	if (k >= LDBL_MIN_EXP) {
		if (k == LDBL_MAX_EXP)
			RETURNI(t * 2 * 0x1p16383L);
		SET_LDBL_EXPSIGN(twopk, BIAS + k);
		RETURNI(t * twopk);
	} else {
		SET_LDBL_EXPSIGN(twopk, BIAS + k + 10000);
		RETURNI(t * twopk * twom10000);
	}
}

/*
 * Our T1 and T2 are chosen to be approximately the points where method
 * A and method B have the same accuracy.  Tang's T1 and T2 are the
 * points where method A's accuracy changes by a full bit.  For Tang,
 * this drop in accuracy makes method A immediately less accurate than
 * method B, but our larger INTERVALS makes method A 2 bits more
 * accurate so it remains the most accurate method significantly
 * closer to the origin despite losing the full bit in our extended
 * range for it.
 *
 * Split the interval [T1, T2] into two intervals [T1, T3] and [T3, T2].
 * Setting T3 to 0 would require the |x| < 0x1p-113 condition to appear
 * in both subintervals, so set T3 = 2**-5, which places the condition
 * into the [T1, T3] interval.
 *
 * XXX we now do this more to (partially) balance the number of terms
 * in the C and D polys than to avoid checking the condition in both
 * intervals.
 *
 * XXX these micro-optimizations are excessive.
 */
static const double
T1 = -0.1659,				/* ~-30.625/128 * log(2) */
T2 =  0.1659,				/* ~30.625/128 * log(2) */
T3 =  0.03125;

/*
 * Domain [-0.1659, 0.03125], range ~[2.9134e-44, 1.8404e-37]:
 * |(exp(x)-1-x-x**2/2)/x - p(x)| < 2**-122.03
 *
 * XXX none of the long double C or D coeffs except C10 is correctly printed.
 * If you re-print their values in %.35Le format, the result is always
 * different.  For example, the last 2 digits in C3 should be 59, not 67.
 * 67 is apparently from rounding an extra-precision value to 36 decimal
 * places.
 */
static const long double
C3  =  1.66666666666666666666666666666666667e-1L,
C4  =  4.16666666666666666666666666666666645e-2L,
C5  =  8.33333333333333333333333333333371638e-3L,
C6  =  1.38888888888888888888888888891188658e-3L,
C7  =  1.98412698412698412698412697235950394e-4L,
C8  =  2.48015873015873015873015112487849040e-5L,
C9  =  2.75573192239858906525606685484412005e-6L,
C10 =  2.75573192239858906612966093057020362e-7L,
C11 =  2.50521083854417203619031960151253944e-8L,
C12 =  2.08767569878679576457272282566520649e-9L,
C13 =  1.60590438367252471783548748824255707e-10L;

/*
 * XXX this has 1 more coeff than needed.
 * XXX can start the double coeffs but not the double mults at C10.
 * With my coeffs (C10-C17 double; s = best_s):
 * Domain [-0.1659, 0.03125], range ~[-1.1976e-37, 1.1976e-37]:
 * |(exp(x)-1-x-x**2/2)/x - p(x)| ~< 2**-122.65
 */
static const double
C14 =  1.1470745580491932e-11,		/*  0x1.93974a81dae30p-37 */
C15 =  7.6471620181090468e-13,		/*  0x1.ae7f3820adab1p-41 */
C16 =  4.7793721460260450e-14,		/*  0x1.ae7cd18a18eacp-45 */
C17 =  2.8074757356658877e-15,		/*  0x1.949992a1937d9p-49 */
C18 =  1.4760610323699476e-16;		/*  0x1.545b43aabfbcdp-53 */

/*
 * Domain [0.03125, 0.1659], range ~[-2.7676e-37, -1.0367e-38]:
 * |(exp(x)-1-x-x**2/2)/x - p(x)| < 2**-121.44
 */
static const long double
D3  =  1.66666666666666666666666666666682245e-1L,
D4  =  4.16666666666666666666666666634228324e-2L,
D5  =  8.33333333333333333333333364022244481e-3L,
D6  =  1.38888888888888888888887138722762072e-3L,
D7  =  1.98412698412698412699085805424661471e-4L,
D8  =  2.48015873015873015687993712101479612e-5L,
D9  =  2.75573192239858944101036288338208042e-6L,
D10 =  2.75573192239853161148064676533754048e-7L,
D11 =  2.50521083855084570046480450935267433e-8L,
D12 =  2.08767569819738524488686318024854942e-9L,
D13 =  1.60590442297008495301927448122499313e-10L;

/*
 * XXX this has 1 more coeff than needed.
 * XXX can start the double coeffs but not the double mults at D11.
 * With my coeffs (D11-D16 double):
 * Domain [0.03125, 0.1659], range ~[-1.1980e-37, 1.1980e-37]:
 * |(exp(x)-1-x-x**2/2)/x - p(x)| ~< 2**-122.65
 */
static const double
D14 =  1.1470726176204336e-11,		/*  0x1.93971dc395d9ep-37 */
D15 =  7.6478532249581686e-13,		/*  0x1.ae892e3D16fcep-41 */
D16 =  4.7628892832607741e-14,		/*  0x1.ad00Dfe41feccp-45 */
D17 =  3.0524857220358650e-15;		/*  0x1.D7e8d886Df921p-49 */

/**
 * Returns 𝑒ˣ-1.
 */
long double
expm1l(long double x)
{
	union IEEEl2bits u, v;
	long double hx2_hi, hx2_lo, q, r, r1, t, twomk, twopk, x_hi;
	long double x_lo, x2;
	double dr, dx, fn, r2;
	int k, n, n2;
	uint16_t hx, ix;

	DOPRINT_START(&x);

	/* Filter out exceptional cases. */
	u.e = x;
	hx = u.xbits.expsign;
	ix = hx & 0x7fff;
	if (ix >= BIAS + 7) {		/* |x| >= 128 or x is NaN */
		if (ix == BIAS + LDBL_MAX_EXP) {
			if (hx & 0x8000)  /* x is -Inf or -NaN */
				RETURNP(-1 / x - 1);
			RETURNP(x + x);	/* x is +Inf or +NaN */
		}
		if (x > o_threshold)
			RETURNP(huge * huge);
		/*
		 * expm1l() never underflows, but it must avoid
		 * unrepresentable large negative exponents.  We used a
		 * much smaller threshold for large |x| above than in
		 * expl() so as to handle not so large negative exponents
		 * in the same way as large ones here.
		 */
		if (hx & 0x8000)	/* x <= -128 */
			RETURN2P(tiny, -1);	/* good for x < -114ln2 - eps */
	}

	ENTERI();

	if (T1 < x && x < T2) {
		x2 = x * x;
		dx = x;

		if (x < T3) {
			if (ix < BIAS - 113) {	/* |x| < 0x1p-113 */
				/* x (rounded) with inexact if x != 0: */
				RETURNPI(x == 0 ? x :
				    (0x1p200 * x + fabsl(x)) * 0x1p-200);
			}
			q = x * x2 * C3 + x2 * x2 * (C4 + x * (C5 + x * (C6 +
			    x * (C7 + x * (C8 + x * (C9 + x * (C10 +
			    x * (C11 + x * (C12 + x * (C13 +
			    dx * (C14 + dx * (C15 + dx * (C16 +
			    dx * (C17 + dx * C18))))))))))))));
		} else {
			q = x * x2 * D3 + x2 * x2 * (D4 + x * (D5 + x * (D6 +
			    x * (D7 + x * (D8 + x * (D9 + x * (D10 +
			    x * (D11 + x * (D12 + x * (D13 +
			    dx * (D14 + dx * (D15 + dx * (D16 +
			    dx * D17)))))))))))));
		}

		x_hi = (float)x;
		x_lo = x - x_hi;
		hx2_hi = x_hi * x_hi / 2;
		hx2_lo = x_lo * (x + x_hi) / 2;
		if (ix >= BIAS - 7)
			RETURN2PI(hx2_hi + x_hi, hx2_lo + x_lo + q);
		else
			RETURN2PI(x, hx2_lo + q + hx2_hi);
	}

	/* Reduce x to (k*ln2 + endpoint[n2] + r1 + r2). */
	fn = rnint((double)x * INV_L);
	n = irint(fn);
	n2 = (unsigned)n % INTERVALS;
	k = n >> LOG2_INTERVALS;
	r1 = x - fn * L1;
	r2 = fn * -L2;
	r = r1 + r2;

	/* Prepare scale factor. */
	v.e = 1;
	v.xbits.expsign = BIAS + k;
	twopk = v.e;

	/*
	 * Evaluate lower terms of
	 * expl(endpoint[n2] + r1 + r2) = kExplData[n2] * expl(r1 + r2).
	 */
	dr = r;
	q = r2 + r * r * (A2 + r * (A3 + r * (A4 + r * (A5 + r * (A6 +
	    dr * (A7 + dr * (A8 + dr * (A9 + dr * A10))))))));

	t = kExplData[n2].lo + kExplData[n2].hi;

	if (k == 0) {
		t = SUM2P(kExplData[n2].hi - 1, kExplData[n2].lo * (r1 + 1) + t * q +
		    kExplData[n2].hi * r1);
		RETURNI(t);
	}
	if (k == -1) {
		t = SUM2P(kExplData[n2].hi - 2, kExplData[n2].lo * (r1 + 1) + t * q +
		    kExplData[n2].hi * r1);
		RETURNI(t / 2);
	}
	if (k < -7) {
		t = SUM2P(kExplData[n2].hi, kExplData[n2].lo + t * (q + r1));
		RETURNI(t * twopk - 1);
	}
	if (k > 2 * LDBL_MANT_DIG - 1) {
		t = SUM2P(kExplData[n2].hi, kExplData[n2].lo + t * (q + r1));
		if (k == LDBL_MAX_EXP)
			RETURNI(t * 2 * 0x1p16383L - 1);
		RETURNI(t * twopk - 1);
	}

	v.xbits.expsign = BIAS - k;
	twomk = v.e;

	if (k > LDBL_MANT_DIG - 1)
		t = SUM2P(kExplData[n2].hi, kExplData[n2].lo - twomk + t * (q + r1));
	else
		t = SUM2P(kExplData[n2].hi - twomk, kExplData[n2].lo + t * (q + r1));
	RETURNI(t * twopk);
}

#endif
