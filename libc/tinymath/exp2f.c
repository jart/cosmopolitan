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

/*
EXP2F_TABLE_BITS = 5
EXP2F_POLY_ORDER = 3

ULP error: 0.502 (nearest rounding.)
Relative error: 1.69 * 2^-34 in [-1/64, 1/64] (before rounding.)
Wrong count: 168353 (all nearest rounding wrong results with fma.)
Non-nearest ULP error: 1 (rounded ULP error)
*/

#define N (1 << EXP2F_TABLE_BITS)
#define T __exp2f_data.tab
#define C __exp2f_data.poly
#define SHIFT __exp2f_data.shift_scaled

static inline uint32_t
top12 (float x)
{
  return asuint (x) >> 20;
}

/**
 * Returns 2^𝑥.
 *
 * - ULP error: 0.502 (nearest rounding.)
 * - Relative error: 1.69 * 2^-34 in [-1/64, 1/64] (before rounding.)
 * - Wrong count: 168353 (all nearest rounding wrong results with fma.)
 * - Non-nearest ULP error: 1 (rounded ULP error)
 */
float
exp2f (float x)
{
  uint32_t abstop;
  uint64_t ki, t;
  /* double_t for better performance on targets with FLT_EVAL_METHOD==2.  */
  double_t kd, xd, z, r, r2, y, s;

  xd = (double_t) x;
  abstop = top12 (x) & 0x7ff;
  if (unlikely (abstop >= top12 (128.0f)))
    {
      /* |x| >= 128 or x is nan.  */
      if (asuint (x) == asuint (-INFINITY))
	return 0.0f;
      if (abstop >= top12 (INFINITY))
	return x + x;
      if (x > 0.0f)
	return __math_oflowf (0);
      if (x <= -150.0f)
	return __math_uflowf (0);
#if WANT_ERRNO_UFLOW
      if (x < -149.0f)
	return __math_may_uflowf (0);
#endif
    }

  /* x = k/N + r with r in [-1/(2N), 1/(2N)] and int k.  */
  kd = eval_as_double (xd + SHIFT);
  ki = asuint64 (kd);
  kd -= SHIFT; /* k/N for int k.  */
  r = xd - kd;

  /* exp2(x) = 2^(k/N) * 2^r ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
  t = T[ki % N];
  t += ki << (52 - EXP2F_TABLE_BITS);
  s = asdouble (t);
  z = C[0] * r + C[1];
  r2 = r * r;
  y = C[2] * r + 1;
  y = z * r2 + y;
  y = y * s;
  return eval_as_float (y);
}

#if USE_GLIBC_ABI
strong_alias (exp2f, __exp2f_finite)
hidden_alias (exp2f, __ieee754_exp2f)
#endif
