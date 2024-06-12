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
#include <limits.h>
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
  TEST(OK, hypotf(3, 4) == 5);
  TEST(OK, hypotf(0, 0) == 0);
  TEST(OK, hypotf(5, 12) == 13);
  TEST(OK, hypotf(-5, -12) == 13);
  TEST(OK, hypotf(-3, 4) == 5);

  // test with zeros
  TEST(OK, hypotf(0, 0) == 0);
  TEST(OK, hypotf(0, 3) == 3);
  TEST(OK, hypotf(3, 0) == 3);

  // test with NAN
  TEST(OK, isnan(hypotf(NAN, 1)));
  TEST(OK, isnan(hypotf(1, NAN)));
  TEST(OK, isnan(hypotf(NAN, NAN)));

  // test underflow avoidance
  TEST(OK, hypotf(2e-38, 3e-38) > 0);

  // test what happens on overflow
  // TODO(jart): This should raise ERANGE.
  TEST(OK, hypotf(FLT_MAX, FLT_MAX) == INFINITY);

  // test with INFINITY
  TEST(OK, hypotf(INFINITY, 1) == INFINITY);
  TEST(OK, hypotf(1, INFINITY) == INFINITY);
  TEST(OK, hypotf(-INFINITY, -INFINITY) == INFINITY);

  // test accuracy assuming hypotl() is correct
  union {
    int i;
    float f;
  } x, y, a, b;
  int n = 1000;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      x.i = rando();
      y.i = rando();
      a.f = hypotl(x.f, y.f);
      b.f = hypotf(x.f, y.f);
      if (isnan(a.f) || isnan(b.f)) {
        CHECK(isnan(a.f) == isnan(b.f));
        continue;
      }
      long e = b.i - a.i;
      if (e < 0)
        e = -e;
      CHECK(e <= MAX_ERROR_ULP);
    }
  }
}
