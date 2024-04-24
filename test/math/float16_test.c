/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2024 Justine Alexandra Roberts Tunney                              │
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

#define CHECK(x) \
  if (!(x))      \
  return __LINE__
#define FALSE(x)          \
  {                       \
    volatile bool x_ = x; \
    if (x_)               \
      return __LINE__;    \
  }
#define TRUE(x)           \
  {                       \
    volatile bool x_ = x; \
    if (!x_)              \
      return __LINE__;    \
  }

_Float16 identity(_Float16 x) {
  return x;
}
_Float16 (*half)(_Float16) = identity;

int main() {
  volatile float f;
  volatile double d;
  volatile _Float16 pi = 3.141;

  // half → float → half
  f = pi;
  pi = f;

  // half → float
  float __extendhfsf2(_Float16);
  CHECK(0.f == __extendhfsf2(0));
  CHECK(3.140625f == __extendhfsf2(pi));
  CHECK(3.140625f == pi);

  // half → double → half
  d = pi;
  pi = d;

  // half → double
  double __extendhfdf2(_Float16);
  CHECK(0. == __extendhfdf2(0));
  CHECK(3.140625 == __extendhfdf2(pi));

  // float → half
  _Float16 __truncsfhf2(float);
  CHECK(0 == (float)__truncsfhf2(0));
  CHECK(pi == (float)__truncsfhf2(3.141f));
  CHECK(3.140625f == (float)__truncsfhf2(3.141f));

  // double → half
  _Float16 __truncdfhf2(double);
  CHECK(0 == (double)__truncdfhf2(0));
  CHECK(3.140625 == (double)__truncdfhf2(3.141));

  // specials
  volatile _Float16 nan = NAN;
  volatile _Float16 positive_infinity = +INFINITY;
  volatile _Float16 negative_infinity = -INFINITY;
  CHECK(isnan(nan));
  CHECK(!isinf(pi));
  CHECK(!isnan(pi));
  CHECK(isinf(positive_infinity));
  CHECK(isinf(negative_infinity));
  CHECK(!isnan(positive_infinity));
  CHECK(!isnan(negative_infinity));
  CHECK(!signbit(pi));
  CHECK(signbit(half(-pi)));
  CHECK(!signbit(half(+0.)));
  CHECK(signbit(half(-0.)));

  // arithmetic
  CHECK(half(-3) == -half(3));
  CHECK(half(9) == half(3) * half(3));
  CHECK(half(0) == half(pi) - half(pi));
  CHECK(half(6.28125) == half(pi) + half(pi));

  // comparisons
  CHECK(half(3) > half(2));
  CHECK(half(3) < half(4));
  CHECK(half(3) <= half(3));
  CHECK(half(3) >= half(3));
  TRUE(half(NAN) != half(NAN));
  FALSE(half(NAN) == half(NAN));
  TRUE(half(3) != half(NAN));
  FALSE(half(3) == half(NAN));
  TRUE(half(NAN) != half(3));
  FALSE(half(NAN) == half(3));
}
