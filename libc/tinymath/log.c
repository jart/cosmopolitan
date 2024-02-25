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

#define T __log_data.tab
#define T2 __log_data.tab2
#define B __log_data.poly1
#define A __log_data.poly
#define Ln2hi __log_data.ln2hi
#define Ln2lo __log_data.ln2lo
#define N (1 << LOG_TABLE_BITS)
#define OFF 0x3fe6000000000000

/* Top 16 bits of a double.  */
static inline uint32_t
top16 (double x)
{
  return asuint64 (x) >> 48;
}

/**
 * Returns natural logarithm of 𝑥.
 *
 * @raise EDOM and FE_INVALID if x is negative
 * @raise ERANGE and FE_DIVBYZERO if x is zero
 */
double
log (double x)
{
  /* double_t for better performance on targets with FLT_EVAL_METHOD==2.  */
  double_t w, z, r, r2, r3, y, invc, logc, kd, hi, lo;
  uint64_t ix, iz, tmp;
  uint32_t top;
  int k, i;

  ix = asuint64 (x);
  top = top16 (x);

#if LOG_POLY1_ORDER == 10 || LOG_POLY1_ORDER == 11
# define LO asuint64 (1.0 - 0x1p-5)
# define HI asuint64 (1.0 + 0x1.1p-5)
#elif LOG_POLY1_ORDER == 12
# define LO asuint64 (1.0 - 0x1p-4)
# define HI asuint64 (1.0 + 0x1.09p-4)
#endif
  if (unlikely (ix - LO < HI - LO))
    {
      /* Handle close to 1.0 inputs separately.  */
      /* Fix sign of zero with downward rounding when x==1.  */
      if (WANT_ROUNDING && unlikely (ix == asuint64 (1.0)))
	return 0;
      r = x - 1.0;
      r2 = r * r;
      r3 = r * r2;
#if LOG_POLY1_ORDER == 10
      /* Worst-case error is around 0.516 ULP.  */
      y = r3 * (B[1] + r * B[2] + r2 * B[3]
		+ r3 * (B[4] + r * B[5] + r2 * B[6] + r3 * (B[7] + r * B[8])));
      w = B[0] * r2; /* B[0] == -0.5.  */
      hi = r + w;
      y += r - hi + w;
      y += hi;
#elif LOG_POLY1_ORDER == 11
      /* Worst-case error is around 0.516 ULP.  */
      y = r3 * (B[1] + r * B[2]
		+ r2 * (B[3] + r * B[4] + r2 * B[5]
			+ r3 * (B[6] + r * B[7] + r2 * B[8] + r3 * B[9])));
      w = B[0] * r2; /* B[0] == -0.5.  */
      hi = r + w;
      y += r - hi + w;
      y += hi;
#elif LOG_POLY1_ORDER == 12
      y = r3 * (B[1] + r * B[2] + r2 * B[3]
		+ r3 * (B[4] + r * B[5] + r2 * B[6]
			+ r3 * (B[7] + r * B[8] + r2 * B[9] + r3 * B[10])));
# if N <= 64
      /* Worst-case error is around 0.532 ULP.  */
      w = B[0] * r2; /* B[0] == -0.5.  */
      hi = r + w;
      y += r - hi + w;
      y += hi;
# else
      /* Worst-case error is around 0.507 ULP.  */
      w = r * 0x1p27;
      double_t rhi = r + w - w;
      double_t rlo = r - rhi;
      w = rhi * rhi * B[0]; /* B[0] == -0.5.  */
      hi = r + w;
      lo = r - hi + w;
      lo += B[0] * rlo * (rhi + r);
      y += lo;
      y += hi;
# endif
#endif
      return eval_as_double (y);
    }
  if (unlikely (top - 0x0010 >= 0x7ff0 - 0x0010))
    {
      /* x < 0x1p-1022 or inf or nan.  */
      if (ix * 2 == 0)
	return __math_divzero (1);
      if (ix == asuint64 (INFINITY)) /* log(inf) == inf.  */
	return x;
      if ((top & 0x8000) || (top & 0x7ff0) == 0x7ff0)
	return __math_invalid (x);
      /* x is subnormal, normalize it.  */
      ix = asuint64 (x * 0x1p52);
      ix -= 52ULL << 52;
    }

  /* x = 2^k z; where z is in range [OFF,2*OFF) and exact.
     The range is split into N subintervals.
     The ith subinterval contains z and c is near its center.  */
  tmp = ix - OFF;
  i = (tmp >> (52 - LOG_TABLE_BITS)) % N;
  k = (int64_t) tmp >> 52; /* arithmetic shift */
  iz = ix - (tmp & 0xfffULL << 52);
  invc = T[i].invc;
  logc = T[i].logc;
  z = asdouble (iz);

  /* log(x) = log1p(z/c-1) + log(c) + k*Ln2.  */
  /* r ~= z/c - 1, |r| < 1/(2*N).  */
#if HAVE_FAST_FMA
  /* rounding error: 0x1p-55/N.  */
  r = fma (z, invc, -1.0);
#else
  /* rounding error: 0x1p-55/N + 0x1p-66.  */
  r = (z - T2[i].chi - T2[i].clo) * invc;
#endif
  kd = (double_t) k;

  /* hi + lo = r + log(c) + k*Ln2.  */
  w = kd * Ln2hi + logc;
  hi = w + r;
  lo = w - hi + r + kd * Ln2lo;

  /* log(x) = lo + (log1p(r) - r) + hi.  */
  r2 = r * r; /* rounding error: 0x1p-54/N^2.  */
  /* Worst case error if |y| > 0x1p-5:
     0.5 + 4.13/N + abs-poly-error*2^57 ULP (+ 0.002 ULP without fma)
     Worst case error if |y| > 0x1p-4:
     0.5 + 2.06/N + abs-poly-error*2^56 ULP (+ 0.001 ULP without fma).  */
#if LOG_POLY_ORDER == 6
  y = lo + r2 * A[0] + r * r2 * (A[1] + r * A[2] + r2 * (A[3] + r * A[4])) + hi;
#elif LOG_POLY_ORDER == 7
  y = lo
      + r2 * (A[0] + r * A[1] + r2 * (A[2] + r * A[3])
	      + r2 * r2 * (A[4] + r * A[5]))
      + hi;
#endif
  return eval_as_double (y);
}

#if USE_GLIBC_ABI
strong_alias (log, __log_finite)
hidden_alias (log, __ieee754_log)
# if LDBL_MANT_DIG == 53
long double logl (long double x) { return log (x); }
# endif
#endif
