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

#define TwoOverSqrtPiMinusOne 0x1.06eba8214db69p-3
#define C 0x1.b0ac16p-1
#define PA __erf_data.erf_poly_A
#define NA __erf_data.erf_ratio_N_A
#define DA __erf_data.erf_ratio_D_A
#define NB __erf_data.erf_ratio_N_B
#define DB __erf_data.erf_ratio_D_B
#define PC __erf_data.erfc_poly_C
#define PD __erf_data.erfc_poly_D
#define PE __erf_data.erfc_poly_E
#define PF __erf_data.erfc_poly_F

/* Top 32 bits of a double.  */
static inline uint32_t
top32 (double x)
{
  return asuint64 (x) >> 32;
}

/**
 * Returns error function of x.
 *
 * Highest measured error is 1.01 ULPs at 0x1.39956ac43382fp+0.
 *
 * @raise ERANGE on underflow
 */
double
erf (double x)
{
  /* Get top word and sign.  */
  uint32_t ix = top32 (x);
  uint32_t ia = ix & 0x7fffffff;
  uint32_t sign = ix >> 31;

  /* Normalized and subnormal cases */
  if (ia < 0x3feb0000)
    { /* a = |x| < 0.84375.  */

      if (ia < 0x3e300000)
	{ /* a < 2^(-28).  */
	  if (ia < 0x00800000)
	    { /* a < 2^(-1015).  */
	      double y =  fma (TwoOverSqrtPiMinusOne, x, x);
	      return check_uflow (y);
	    }
	  return x + TwoOverSqrtPiMinusOne * x;
	}

      double x2 = x * x;

      if (ia < 0x3fe00000)
	{ /* a < 0.5  - Use polynomial approximation.  */
	  double r1 = fma (x2, PA[1], PA[0]);
	  double r2 = fma (x2, PA[3], PA[2]);
	  double r3 = fma (x2, PA[5], PA[4]);
	  double r4 = fma (x2, PA[7], PA[6]);
	  double r5 = fma (x2, PA[9], PA[8]);
	  double x4 = x2 * x2;
	  double r = r5;
	  r = fma (x4, r, r4);
	  r = fma (x4, r, r3);
	  r = fma (x4, r, r2);
	  r = fma (x4, r, r1);
	  return fma (r, x, x); /* This fma is crucial for accuracy.  */
	}
      else
	{ /* 0.5 <= a < 0.84375 - Use rational approximation.  */
	  double x4, x8, r1n, r2n, r1d, r2d, r3d;

	  r1n = fma (x2, NA[1], NA[0]);
	  x4 = x2 * x2;
	  r2n = fma (x2, NA[3], NA[2]);
	  x8 = x4 * x4;
	  r1d = fma (x2, DA[0], 1.0);
	  r2d = fma (x2, DA[2], DA[1]);
	  r3d = fma (x2, DA[4], DA[3]);
	  double P = r1n + x4 * r2n + x8 * NA[4];
	  double Q = r1d + x4 * r2d + x8 * r3d;
	  return fma (P / Q, x, x);
	}
    }
  else if (ia < 0x3ff40000)
    { /* 0.84375 <= |x| < 1.25.  */
      double a2, a4, a6, r1n, r2n, r3n, r4n, r1d, r2d, r3d, r4d;
      double a = fabs (x) - 1.0;
      r1n = fma (a, NB[1], NB[0]);
      a2 = a * a;
      r1d = fma (a, DB[0], 1.0);
      a4 = a2 * a2;
      r2n = fma (a, NB[3], NB[2]);
      a6 = a4 * a2;
      r2d = fma (a, DB[2], DB[1]);
      r3n = fma (a, NB[5], NB[4]);
      r3d = fma (a, DB[4], DB[3]);
      r4n = NB[6];
      r4d = DB[5];
      double P = r1n + a2 * r2n + a4 * r3n + a6 * r4n;
      double Q = r1d + a2 * r2d + a4 * r3d + a6 * r4d;
      if (sign)
	return -C - P / Q;
      else
	return C + P / Q;
    }
  else if (ia < 0x40000000)
    { /* 1.25 <= |x| < 2.0.  */
      double a = fabs (x);
      a = a - 1.25;

      double r1 = fma (a, PC[1], PC[0]);
      double r2 = fma (a, PC[3], PC[2]);
      double r3 = fma (a, PC[5], PC[4]);
      double r4 = fma (a, PC[7], PC[6]);
      double r5 = fma (a, PC[9], PC[8]);
      double r6 = fma (a, PC[11], PC[10]);
      double r7 = fma (a, PC[13], PC[12]);
      double r8 = fma (a, PC[15], PC[14]);

      double a2 = a * a;

      double r = r8;
      r = fma (a2, r, r7);
      r = fma (a2, r, r6);
      r = fma (a2, r, r5);
      r = fma (a2, r, r4);
      r = fma (a2, r, r3);
      r = fma (a2, r, r2);
      r = fma (a2, r, r1);

      if (sign)
	return -1.0 + r;
      else
	return 1.0 - r;
    }
  else if (ia < 0x400a0000)
    { /* 2 <= |x| < 3.25.  */
      double a = fabs (x);
      a = fma (0.5, a, -1.0);

      double r1 = fma (a, PD[1], PD[0]);
      double r2 = fma (a, PD[3], PD[2]);
      double r3 = fma (a, PD[5], PD[4]);
      double r4 = fma (a, PD[7], PD[6]);
      double r5 = fma (a, PD[9], PD[8]);
      double r6 = fma (a, PD[11], PD[10]);
      double r7 = fma (a, PD[13], PD[12]);
      double r8 = fma (a, PD[15], PD[14]);
      double r9 = fma (a, PD[17], PD[16]);

      double a2 = a * a;

      double r = r9;
      r = fma (a2, r, r8);
      r = fma (a2, r, r7);
      r = fma (a2, r, r6);
      r = fma (a2, r, r5);
      r = fma (a2, r, r4);
      r = fma (a2, r, r3);
      r = fma (a2, r, r2);
      r = fma (a2, r, r1);

      if (sign)
	return -1.0 + r;
      else
	return 1.0 - r;
    }
  else if (ia < 0x40100000)
    { /* 3.25 <= |x| < 4.0.  */
      double a = fabs (x);
      a = a - 3.25;

      double r1 = fma (a, PE[1], PE[0]);
      double r2 = fma (a, PE[3], PE[2]);
      double r3 = fma (a, PE[5], PE[4]);
      double r4 = fma (a, PE[7], PE[6]);
      double r5 = fma (a, PE[9], PE[8]);
      double r6 = fma (a, PE[11], PE[10]);
      double r7 = fma (a, PE[13], PE[12]);

      double a2 = a * a;

      double r = r7;
      r = fma (a2, r, r6);
      r = fma (a2, r, r5);
      r = fma (a2, r, r4);
      r = fma (a2, r, r3);
      r = fma (a2, r, r2);
      r = fma (a2, r, r1);

      if (sign)
	return -1.0 + r;
      else
	return 1.0 - r;
    }
  else if (ia < 0x4017a000)
    { /* 4 <= |x| < 5.90625.  */
      double a = fabs (x);
      a = fma (0.5, a, -2.0);

      double r1 = fma (a, PF[1], PF[0]);
      double r2 = fma (a, PF[3], PF[2]);
      double r3 = fma (a, PF[5], PF[4]);
      double r4 = fma (a, PF[7], PF[6]);
      double r5 = fma (a, PF[9], PF[8]);
      double r6 = fma (a, PF[11], PF[10]);
      double r7 = fma (a, PF[13], PF[12]);
      double r8 = fma (a, PF[15], PF[14]);
      double r9 = PF[16];

      double a2 = a * a;

      double r = r9;
      r = fma (a2, r, r8);
      r = fma (a2, r, r7);
      r = fma (a2, r, r6);
      r = fma (a2, r, r5);
      r = fma (a2, r, r4);
      r = fma (a2, r, r3);
      r = fma (a2, r, r2);
      r = fma (a2, r, r1);

      if (sign)
	return -1.0 + r;
      else
	return 1.0 - r;
    }
  else
    {
      /* Special cases : erf(nan)=nan, erf(+inf)=+1 and erf(-inf)=-1.  */
      if (unlikely (ia >= 0x7ff00000))
	return (double) (1.0 - (sign << 1)) + 1.0 / x;

      if (sign)
	return -1.0;
      else
	return 1.0;
    }
}

#if LDBL_MANT_DIG == 53
__weak_reference(erf, erfl);
#endif
