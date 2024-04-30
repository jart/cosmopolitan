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
#include "libc/cosmo.h"
#include "libc/tinymath/arm.internal.h"

#if WANT_ERRNO
/* dontinline reduces code size and avoids making math functions non-leaf
   when the error handling is inlined.  */
dontinline static float
with_errnof (float y, int e)
{
  errno = e;
  return y;
}
#else
#define with_errnof(x, e) (x)
#endif

/* dontinline reduces code size.  */
dontinline static float
xflowf (uint32_t sign, float y)
{
  unleaf();
  y = eval_as_float (opt_barrier_float (sign ? -y : y) * y);
  return with_errnof (y, ERANGE);
}

float
__math_uflowf (uint32_t sign)
{
  return xflowf (sign, 0x1p-95f);
}

#if WANT_ERRNO_UFLOW
/* Underflows to zero in some non-nearest rounding mode, setting errno
   is valid even if the result is non-zero, but in the subnormal range.  */
float
__math_may_uflowf (uint32_t sign)
{
  return xflowf (sign, 0x1.4p-75f);
}
#endif

float
__math_oflowf (uint32_t sign)
{
  return xflowf (sign, 0x1p97f);
}

float
__math_divzerof (uint32_t sign)
{
  unleaf();
  float y = opt_barrier_float (sign ? -1.0f : 1.0f) / 0.0f;
  return with_errnof (y, ERANGE);
}

dontinstrument float
__math_invalidf (float x)
{
  unleaf();
  float y = (x - x) / (x - x);
  return isnan (x) ? y : with_errnof (y, EDOM);
}

/* Check result and set errno if necessary.  */

dontinstrument float
__math_check_uflowf (float y)
{
  return y == 0.0f ? with_errnof (y, ERANGE) : y;
}

dontinstrument float
__math_check_oflowf (float y)
{
  return isinf (y) ? with_errnof (y, ERANGE) : y;
}
