// Copyright 2024 Justine Alexandra Roberts Tunney
//
// Permission to use, copy, modify, and/or distribute this software for
// any purpose with or without fee is hereby granted, provided that the
// above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
// WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
// DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
// PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
// TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#include <math.h>

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

__bf16 identity(__bf16 x) {
  return x;
}
__bf16 (*half)(__bf16) = identity;

unsigned toint(float f) {
  union {
    float f;
    unsigned i;
  } u = {f};
  return u.i;
}

int main() {
  volatile float f;
  volatile double d;
  volatile __bf16 pi = 3.141;

  // half → float → half
  f = pi;
  pi = f;

  // half → float
  float __extendbfsf2(__bf16);
  CHECK(0.f == __extendbfsf2(0));
  CHECK(3.140625f == __extendbfsf2(pi));
  CHECK(3.140625f == pi);

  // half → double → half
  d = pi;
  pi = d;

  // float → half
  __bf16 __truncsfbf2(float);
  CHECK(0 == (float)__truncsfbf2(0));
  CHECK(pi == (float)__truncsfbf2(3.141f));
  CHECK(3.140625f == (float)__truncsfbf2(3.141f));

  // double → half
  __bf16 __truncdfbf2(double);
  CHECK(0 == (double)__truncdfbf2(0));
  CHECK(3.140625 == (double)__truncdfbf2(3.141));

  // specials
  volatile __bf16 nan = NAN;
  volatile __bf16 positive_infinity = +INFINITY;
  volatile __bf16 negative_infinity = -INFINITY;
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
