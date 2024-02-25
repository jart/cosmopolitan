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
#include "libc/errno.h"
#include "libc/tinymath/arm.internal.h"

#if WANT_ERRNO
/* dontinline reduces code size and avoids making math functions non-leaf
   when the error handling is inlined.  */
dontinline static double
with_errno (double y, int e)
{
  errno = e;
  return y;
}
#else
#define with_errno(x, e) (x)
#endif

/* dontinline reduces code size.  */
dontinline static double
xflow (uint32_t sign, double y)
{
  y = eval_as_double (opt_barrier_double (sign ? -y : y) * y);
  return with_errno (y, ERANGE);
}

double
__math_uflow (uint32_t sign)
{
  return xflow (sign, 0x1p-767);
}

#if WANT_ERRNO_UFLOW
/* Underflows to zero in some non-nearest rounding mode, setting errno
   is valid even if the result is non-zero, but in the subnormal range.  */
double
__math_may_uflow (uint32_t sign)
{
  return xflow (sign, 0x1.8p-538);
}
#endif

double
__math_oflow (uint32_t sign)
{
  return xflow (sign, 0x1p769);
}

double
__math_divzero (uint32_t sign)
{
  double y = opt_barrier_double (sign ? -1.0 : 1.0) / 0.0;
  return with_errno (y, ERANGE);
}

dontinstrument double
__math_invalid (double x)
{
  double y = (x - x) / (x - x);
  return isnan (x) ? y : with_errno (y, EDOM);
}

/* Check result and set errno if necessary.  */

dontinstrument double
__math_check_uflow (double y)
{
  return y == 0.0 ? with_errno (y, ERANGE) : y;
}

dontinstrument double
__math_check_oflow (double y)
{
  return isinf (y) ? with_errno (y, ERANGE) : y;
}
