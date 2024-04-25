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

long lemur(void) {
  static unsigned __int128 s = 2131259787901769494;
  return (s *= 15750249268501108917ul) >> 64;
}

int main() {

  // test base cases
  TEST(OK, hypot(3, 4) == 5);
  TEST(OK, hypot(0, 0) == 0);
  TEST(OK, hypot(5, 12) == 13);
  TEST(OK, hypot(-5, -12) == 13);
  TEST(OK, hypot(-3, 4) == 5);

  // test with zeros
  TEST(OK, hypot(0, 0) == 0);
  TEST(OK, hypot(0, 3) == 3);
  TEST(OK, hypot(3, 0) == 3);

  // test with NAN
  TEST(OK, isnan(hypot(NAN, 1)));
  TEST(OK, isnan(hypot(1, NAN)));
  TEST(OK, isnan(hypot(NAN, NAN)));

  // test with INFINITY
  TEST(OK, hypot(INFINITY, 1) == INFINITY);
  TEST(OK, hypot(1, INFINITY) == INFINITY);
  TEST(OK, hypot(-INFINITY, -INFINITY) == INFINITY);

  // test underflow avoidance
  TEST(OK, hypot(2e-308, 3e-308) > 0);

  // test what happens on overflow
  // TODO(jart): This should raise ERANGE.
  TEST(OK, hypot(DBL_MAX, DBL_MAX) == INFINITY);

  // test accuracy assuming hypotl() is correct
  union {
    long i;
    double f;
  } x, y, a, b;
  int n = 1000;
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      x.i = lemur();
      y.i = lemur();
      a.f = hypotl(x.f, y.f);
      b.f = hypot(x.f, y.f);
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
