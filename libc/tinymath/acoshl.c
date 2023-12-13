/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│ FreeBSD lib/msun/src/e_acoshl.c                                              │
│ Converted to ldbl by David Schultz <das@FreeBSD.ORG> and Bruce D. Evans.     │
│                                                                              │
│ Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.            │
│                                                                              │
│ Developed at SunPro, a Sun Microsystems, Inc. business.                      │
│ Permission to use, copy, modify, and distribute this                         │
│ software is freely granted, provided that this notice                        │
│ is preserved.                                                                │
│                                                                              │
│ Copyright (c) 1992-2023 The FreeBSD Project.                                 │
│                                                                              │
│ Redistribution and use in source and binary forms, with or without           │
│ modification, are permitted provided that the following conditions           │
│ are met:                                                                     │
│ 1. Redistributions of source code must retain the above copyright            │
│    notice, this list of conditions and the following disclaimer.             │
│ 2. Redistributions in binary form must reproduce the above copyright         │
│    notice, this list of conditions and the following disclaimer in the       │
│    documentation and/or other materials provided with the distribution.      │
│                                                                              │
│ THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND       │
│ ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE        │
│ IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE   │
│ ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE      │
│ FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL   │
│ DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS      │
│ OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)        │
│ HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT   │
│ LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    │
│ OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF       │
│ SUCH DAMAGE.                                                                 │
│                                                                              │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/math.h"
#include "libc/tinymath/freebsd.internal.h"
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)

asm(".ident\t\"\\n\\n\
FreeBSD libm (BSD-2 License)\\n\
Copyright (c) 2005-2011, Bruce D. Evans, Steven G. Kargl, David Schultz.\"");
asm(".ident\t\"\\n\\n\
fdlibm (fdlibm license)\\n\
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

/* EXP_LARGE is the threshold above which we use acosh(x) ~= log(2x). */
#if LDBL_MANT_DIG == 64
#define	EXP_LARGE	34
#elif LDBL_MANT_DIG == 113
#define	EXP_LARGE	58
#else
#error "Unsupported long double format"
#endif

#if LDBL_MAX_EXP != 0x4000
/* We also require the usual expsign encoding. */
#error "Unsupported long double format"
#endif

static const double
one	= 1.0;

#if LDBL_MANT_DIG == 64
static const union IEEEl2bits
u_ln2 =  LD80C(0xb17217f7d1cf79ac, -1, 6.93147180559945309417e-1L);
#define	ln2	u_ln2.e
#elif LDBL_MANT_DIG == 113
static const long double
ln2 =  6.93147180559945309417232121458176568e-1L;	/* 0x162e42fefa39ef35793c7673007e6.0p-113 */
#else
#error "Unsupported long double format"
#endif

/**
 * Returns inverse hyperbolic cosine of 𝑥.
 * @define acosh(x) = log(x + sqrt(x*x-1))
 */
long double
acoshl(long double x)
{
	long double t;
	int16_t hx;

	ENTERI();
	GET_LDBL_EXPSIGN(hx, x);
	if (hx < 0x3fff) {		/* x < 1, or misnormal */
	    RETURNI((x-x)/(x-x));
	} else if (hx >= BIAS + EXP_LARGE) { /* x >= LARGE */
	    if (hx >= 0x7fff) {		/* x is inf, NaN or misnormal */
	        RETURNI(x+x);
	    } else {
		RETURNI(logl(x)+ln2);	/* acosh(huge)=log(2x), or misnormal */
	    }
	} else if (hx == 0x3fff && x == 1) {
	    RETURNI(0.0);		/* acosh(1) = 0 */
	} else if (hx >= 0x4000) {	/* LARGE > x >= 2, or misnormal */
	    t=x*x;
	    RETURNI(logl(2.0*x-one/(x+sqrtl(t-one))));
	} else {			/* 1<x<2 */
	    t = x-one;
	    RETURNI(log1pl(t+sqrtl(2.0*t+t*t)));
	}
}

#endif /* long double is long */
