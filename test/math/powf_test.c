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
#include <errno.h>
#include <float.h>
#include <math.h>

#define OK 0

#define MAX_ERROR_ULP 1

#define CHECK(x) \
  if (!(x))      \
  return __LINE__

#define TEST(e, x) \
  errno = 0;       \
  CHECK(x);        \
  CHECK(errno == e)

int rando(void) {
  static unsigned long s;
  s *= 6364136223846793005;
  s += 1442695040888963407;
  return s >> 32;
}

int main() {

  // test base cases
  TEST(OK, !powf(0, 5));
  TEST(OK, powf(0, 0) == 1);
  TEST(OK, powf(1, 0) == 1);
  TEST(OK, powf(2, 0) == 1);
  TEST(OK, powf(0, 2) == 0);
  TEST(OK, powf(5, 0) == 1);
  TEST(OK, powf(1, 5) == 1);
  TEST(OK, powf(2, 3) == 8);
  TEST(OK, powf(-2, 3) == -8);
  TEST(OK, powf(-2, 2) == 4);
  TEST(OK, powf(2, -2) == 0.25);

  // test edge cases
  TEST(OK, powf(-2, -2) == 0.25);
  TEST(OK, powf(2, .5) == sqrtf(2));
  TEST(OK, powf(2, -.5) == M_SQRT1_2f);

  // test special values
  TEST(OK, powf(NAN, 0) == 1);
  TEST(OK, !powf(INFINITY, -2));
  TEST(OK, isnan(powf(NAN, 2)));
  TEST(OK, isnan(powf(2, NAN)));
  TEST(OK, powf(INFINITY, -1) == 0);
  TEST(OK, powf(INFINITY, 2) == INFINITY);
  TEST(OK, powf(-INFINITY, 2) == INFINITY);
  TEST(OK, powf(-INFINITY, 3) == -INFINITY);

  // test domain errors
  TEST(EDOM, isnan(powf(-1, 0.5)));

  // test pole errors
  TEST(ERANGE, isinf(powf(0, -1)));
  TEST(ERANGE, isinf(powf(0, -.5)));

  // test underflow and overflow
  TEST(ERANGE, powf(1e-38, 2) == 0);
  TEST(ERANGE, powf(FLT_MAX, 2) == INFINITY);

  // test accuracy assuming pow() is correct
  union {
    int i;
    float f;
  } x, y, a, b;
  int n = 1000;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      x.i = rando();
      y.i = rando();
      a.f = pow(x.f, y.f);
      b.f = powf(x.f, y.f);
      if (isnan(a.f) || isnan(b.f)) {
        CHECK(isnan(a.f) == isnan(b.f));
        continue;
      }
      int e = b.i - a.i;
      if (e < 0)
        e = -e;
      CHECK(e <= MAX_ERROR_ULP);
    }
  }
}
