/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:8;tab-width:8;coding:utf-8   -*-│
│ vi: set noet ft=c ts=8 sw=8 fenc=utf-8                                   :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│ FreeBSD lib/msun/src/s_expm1f.c                                              │
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

asm(".ident\t\"\\n\\n\
FreeBSD libm (BSD-2 License)\\n\
Copyright (c) 2005-2011, Bruce D. Evans, Steven G. Kargl, David Schultz.\"");
asm(".ident\t\"\\n\\n\
fdlibm (fdlibm license)\\n\
Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.\"");
asm(".include \"libc/disclaimer.inc\"");
// clang-format off

static const float
one		= 1.0,
tiny		= 1.0e-30,
o_threshold	= 8.8721679688e+01,/* 0x42b17180 */
ln2_hi		= 6.9313812256e-01,/* 0x3f317180 */
ln2_lo		= 9.0580006145e-06,/* 0x3717f7d1 */
invln2		= 1.4426950216e+00,/* 0x3fb8aa3b */
/*
 * Domain [-0.34568, 0.34568], range ~[-6.694e-10, 6.696e-10]:
 * |6 / x * (1 + 2 * (1 / (exp(x) - 1) - 1 / x)) - q(x)| < 2**-30.04
 * Scaled coefficients: Qn_here = 2**n * Qn_for_q (see s_expm1.c):
 */
Q1 = -3.3333212137e-2,		/* -0x888868.0p-28 */
Q2 =  1.5807170421e-3;		/*  0xcf3010.0p-33 */

static volatile float huge = 1.0e+30;

/**
 * Returns 𝑒ˣ-1.
 */
float
expm1f(float x)
{
	float y,hi,lo,c,t,e,hxs,hfx,r1,twopk;
	int32_t k,xsb;
	uint32_t hx;

	GET_FLOAT_WORD(hx,x);
	xsb = hx&0x80000000;		/* sign bit of x */
	hx &= 0x7fffffff;		/* high word of |x| */

	/* filter out huge and non-finite argument */
	if(hx >= 0x4195b844) {			/* if |x|>=27*ln2 */
	    if(hx >= 0x42b17218) {		/* if |x|>=88.721... */
                if(hx>0x7f800000)
		    return x+x; 	 /* NaN */
		if(hx==0x7f800000)
		    return (xsb==0)? x:-1.0;/* exp(+-inf)={inf,-1} */
	        if(x > o_threshold) return huge*huge; /* overflow */
	    }
	    if(xsb!=0) { /* x < -27*ln2, return -1.0 with inexact */
		if(x+tiny<(float)0.0)	/* raise inexact */
		return tiny-one;	/* return -1 */
	    }
	}

	/* argument reduction */
	if(hx > 0x3eb17218) {		/* if  |x| > 0.5 ln2 */
	    if(hx < 0x3F851592) {	/* and |x| < 1.5 ln2 */
		if(xsb==0)
		    {hi = x - ln2_hi; lo =  ln2_lo;  k =  1;}
		else
		    {hi = x + ln2_hi; lo = -ln2_lo;  k = -1;}
	    } else {
		k  = invln2*x+((xsb==0)?(float)0.5:(float)-0.5);
		t  = k;
		hi = x - t*ln2_hi;	/* t*ln2_hi is exact here */
		lo = t*ln2_lo;
	    }
	    STRICT_ASSIGN(float, x, hi - lo);
	    c  = (hi-x)-lo;
	}
	else if(hx < 0x33000000) {  	/* when |x|<2**-25, return x */
	    t = huge+x;	/* return x with inexact flags when x!=0 */
	    return x - (t-(huge+x));
	}
	else k = 0;

	/* x is now in primary range */
	hfx = (float)0.5*x;
	hxs = x*hfx;
	r1 = one+hxs*(Q1+hxs*Q2);
	t  = (float)3.0-r1*hfx;
	e  = hxs*((r1-t)/((float)6.0 - x*t));
	if(k==0) return x - (x*e-hxs);		/* c is 0 */
	else {
	    SET_FLOAT_WORD(twopk,((uint32_t)(0x7f+k))<<23);	/* 2^k */
	    e  = (x*(e-c)-c);
	    e -= hxs;
	    if(k== -1) return (float)0.5*(x-e)-(float)0.5;
	    if(k==1) {
	       	if(x < (float)-0.25) return -(float)2.0*(e-(x+(float)0.5));
	       	else 	      return  one+(float)2.0*(x-e);
	    }
	    if (k <= -2 || k>56) {   /* suffice to return exp(x)-1 */
	        y = one-(e-x);
		if (k == 128) y = y*2.0F*0x1p127F;
		else y = y*twopk;
	        return y-one;
	    }
	    t = one;
	    if(k<23) {
	        SET_FLOAT_WORD(t,0x3f800000 - (0x1000000>>k)); /* t=1-2^-k */
	       	y = t-(e-x);
		y = y*twopk;
	   } else {
		SET_FLOAT_WORD(t,((0x7f-k)<<23));	/* 2^-k */
	       	y = x-(e+t);
	       	y += one;
		y = y*twopk;
	    }
	}
	return y;
}
