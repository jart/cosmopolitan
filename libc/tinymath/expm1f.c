/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╚──────────────────────────────────────────────────────────────────────────────╝
│                                                                              │
│  Optimized Routines                                                          │
│  Copyright (c) 1999-2022, Arm Limited.                                       │
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
#include "libc/tinymath/hornerf.internal.h"
#include "libc/tinymath/internal.h"
#include "third_party/libcxx/math.h"

asm(".ident\t\"\\n\\n\
Optimized Routines (MIT License)\\n\
Copyright 2022 ARM Limited\"");
asm(".include \"libc/disclaimer.inc\"");
/* clang-format off */

#define Shift (0x1.8p23f)
#define InvLn2 (0x1.715476p+0f)
#define Ln2hi (0x1.62e4p-1f)
#define Ln2lo (0x1.7f7d1cp-20f)
#define AbsMask (0x7fffffff)
#define InfLimit                                                               \
  (0x1.644716p6) /* Smallest value of x for which expm1(x) overflows.  */
#define NegLimit                                                               \
  (-0x1.9bbabcp+6) /* Largest value of x for which expm1(x) rounds to 1.  */

#define C(i) __expm1f_poly[i]

/* Generated using fpminimax, see tools/expm1f.sollya for details.  */
const float __expm1f_poly[] = {0x1.fffffep-2, 0x1.5554aep-3, 0x1.555736p-5,
			       0x1.12287cp-7, 0x1.6b55a2p-10};

/* Approximation for exp(x) - 1 using polynomial on a reduced interval.
   The maximum error is 1.51 ULP:
   expm1f(0x1.8baa96p-2) got 0x1.e2fb9p-2
			want 0x1.e2fb94p-2.  */
float
expm1f (float x)
{
  uint32_t ix = asuint (x);
  uint32_t ax = ix & AbsMask;

  /* Tiny: |x| < 0x1p-23. expm1(x) is closely approximated by x.
     Inf:  x == +Inf => expm1(x) = x.  */
  if (ax <= 0x34000000 || (ix == 0x7f800000))
    return x;

  /* +/-NaN.  */
  if (ax > 0x7f800000)
    return __math_invalidf (x);

  if (x >= InfLimit)
    return __math_oflowf (0);

  if (x <= NegLimit || ix == 0xff800000)
    return -1;

  /* Reduce argument to smaller range:
     Let i = round(x / ln2)
     and f = x - i * ln2, then f is in [-ln2/2, ln2/2].
     exp(x) - 1 = 2^i * (expm1(f) + 1) - 1
     where 2^i is exact because i is an integer.  */
  float j = fmaf (InvLn2, x, Shift) - Shift;
  int32_t i = j;
  float f = fmaf (j, -Ln2hi, x);
  f = fmaf (j, -Ln2lo, f);

  /* Approximate expm1(f) using polynomial.
     Taylor expansion for expm1(x) has the form:
	 x + ax^2 + bx^3 + cx^4 ....
     So we calculate the polynomial P(f) = a + bf + cf^2 + ...
     and assemble the approximation expm1(f) ~= f + f^2 * P(f).  */
  float p = fmaf (f * f, HORNER_4 (f, C), f);
  /* Assemble the result, using a slight rearrangement to achieve acceptable
     accuracy.
     expm1(x) ~= 2^i * (p + 1) - 1
     Let t = 2^(i - 1).  */
  float t = ldexpf (0.5f, i);
  /* expm1(x) ~= 2 * (p * t + (t - 1/2)).  */
  return 2 * fmaf (p, t, t - 0.5f);
}
