/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

float remainderf2(float, float);
double _remainder(double, double) asm("remainder");
float _remainderf(float, float) asm("remainderf");
long double _remainderl(long double, long double) asm("remainderl");

TEST(remainder, test) {
  EXPECT_TRUE(isnan(_remainder(7, NAN)));
  EXPECT_TRUE(isnan(_remainder(NAN, 7)));
  EXPECT_TRUE(isnan(_remainder(INFINITY, 7)));
  EXPECT_TRUE(isnan(_remainder(7, 0)));
  EXPECT_STREQ("-1", gc(xdtoa(_remainder(7, 4))));
}

TEST(remainderf, test) {
  EXPECT_TRUE(isnan(_remainderf(7, NAN)));
  EXPECT_TRUE(isnan(_remainderf(NAN, 7)));
  EXPECT_TRUE(isnan(_remainderf(INFINITY, 7)));
  EXPECT_TRUE(isnan(_remainderf(7, 0)));
  EXPECT_STREQ("-1", gc(xdtoaf(_remainderf(7, 4))));
}

TEST(remainderl, test) {
  EXPECT_TRUE(isnan(_remainderl(7, NAN)));
  EXPECT_TRUE(isnan(_remainderl(NAN, 7)));
  EXPECT_TRUE(isnan(_remainderl(INFINITY, 7)));
  EXPECT_TRUE(isnan(_remainderl(7, 0)));
  EXPECT_STREQ("-1", gc(xdtoal(_remainderl(7, 4))));
}

static inline float i2f(uint32_t i) {
  union {
    uint32_t i;
    float f;
  } u = {i};
  return u.f;
}

static inline uint32_t f2i(float f) {
  union {
    float f;
    uint32_t i;
  } u = {f};
  return u.i;
}

BENCH(remainder, bench) {
  EZBENCH2("remainder", donothing, _remainder(7, 3));    //  ~20ns
  EZBENCH2("remainderf", donothing, _remainderf(7, 3));  //  ~20ns
  EZBENCH2("remainderl", donothing, _remainderl(7, 3));  // ~169ns
}

#if 0
TEST(remainderf, brute) {
  long i, j;
  float a, b;
  int lim = 100;
  uint32_t x, y;
  for (i = 0; i <= 0x100000000; i += 1048573) {
    for (j = 0; j <= 0x100000000; j += 1048573) {
      a = remainderf(i2f(i), i2f(j));
      b = remainderf2(i2f(i), i2f(j));
      if (isunordered(a, b)) {
        assert(isnan(a) == isnan(b));
        assert(isfinite(a) == isfinite(b));
        continue;
      }
      x = f2i(a);
      y = f2i(b);
      if (abs(x - y) > 1) {
        kprintf("bah %#lx %s %#lx %d\n", i, gc(xdtoaf(i2f(i))), j,
                gc(xdtoaf(i2f(j))), abs(x - y));
        kprintf(" %-12s %#x\n", gc(xdtoaf(i2f(x))), x);
        kprintf(" %-12s %#x\n", gc(xdtoaf(i2f(y))), y);
        if (!--lim) break;
      }
    }
    kprintf("%ld\r", i);
  }
  if (lim != 100) {
    exit(1);
  }
}
#endif
