/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Optimized Routines                                                          │
│  Copyright (c) 2018-2024, Arm Limited.                                       │
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
#include "libc/tinymath/arm.internal.h"
__static_yoink("arm_optimized_routines_notice");

#define TwoOverSqrtPiMinusOne 0x1.06eba8p-3f
#define A __erff_data.erff_poly_A
#define B __erff_data.erff_poly_B

/* Top 12 bits of a float.  */
static inline uint32_t
top12 (float x)
{
  return asuint (x) >> 20;
}

/* Efficient implementation of erff
   using either a pure polynomial approximation or
   the exponential of a polynomial.
   Worst-case error is 1.09ulps at 0x1.c111acp-1.  */
float
erff (float x)
{
  float r, x2, u;

  /* Get top word.  */
  uint32_t ix = asuint (x);
  uint32_t sign = ix >> 31;
  uint32_t ia12 = top12 (x) & 0x7ff;

  /* Limit of both intervals is 0.875 for performance reasons but coefficients
     computed on [0.0, 0.921875] and [0.921875, 4.0], which brought accuracy
     from 0.94 to 1.1ulps.  */
  if (ia12 < 0x3f6)
    { /* a = |x| < 0.875.  */

      /* Tiny and subnormal cases.  */
      if (unlikely (ia12 < 0x318))
	{ /* |x| < 2^(-28).  */
	  if (unlikely (ia12 < 0x040))
	    { /* |x| < 2^(-119).  */
	      float y = fmaf (TwoOverSqrtPiMinusOne, x, x);
	      return check_uflowf (y);
	    }
	  return x + TwoOverSqrtPiMinusOne * x;
	}

      x2 = x * x;

      /* Normalized cases (|x| < 0.921875). Use Horner scheme for x+x*P(x^2).  */
      r = A[5];
      r = fmaf (r, x2, A[4]);
      r = fmaf (r, x2, A[3]);
      r = fmaf (r, x2, A[2]);
      r = fmaf (r, x2, A[1]);
      r = fmaf (r, x2, A[0]);
      r = fmaf (r, x, x);
    }
  else if (ia12 < 0x408)
    { /* |x| < 4.0 - Use a custom Estrin scheme.  */

      float a = fabsf (x);
      /* Start with Estrin scheme on high order (small magnitude) coefficients.  */
      r = fmaf (B[6], a, B[5]);
      u = fmaf (B[4], a, B[3]);
      x2 = x * x;
      r = fmaf (r, x2, u);
      /* Then switch to pure Horner scheme.  */
      r = fmaf (r, a, B[2]);
      r = fmaf (r, a, B[1]);
      r = fmaf (r, a, B[0]);
      r = fmaf (r, a, a);
      /* Single precision exponential with ~0.5ulps,
	 ensures erff has max. rel. error
	 < 1ulp on [0.921875, 4.0],
	 < 1.1ulps on [0.875, 4.0].  */
      r = expf (-r);
      /* Explicit copysign (calling copysignf increases latency).  */
      if (sign)
	r = -1.0f + r;
      else
	r = 1.0f - r;
    }
  else
    { /* |x| >= 4.0.  */

      /* Special cases : erff(nan)=nan, erff(+inf)=+1 and erff(-inf)=-1.  */
      if (unlikely (ia12 >= 0x7f8))
	return (1.f - (float) ((ix >> 31) << 1)) + 1.f / x;

      /* Explicit copysign (calling copysignf increases latency).  */
      if (sign)
	r = -1.0f;
      else
	r = 1.0f;
    }
  return r;
}
