/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/conv.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

double _tanh(double) asm("tanh");
float _tanhf(float) asm("tanhf");
long double _tanhl(long double) asm("tanhl");

TEST(tanh, test) {
  EXPECT_STREQ("0", gc(xasprintf("%.15g", _tanh(0.))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", _tanh(-0.))));
  EXPECT_STREQ("0.0996679946249558", gc(xasprintf("%.15g", _tanh(.1))));
  EXPECT_STREQ("-0.0996679946249558", gc(xasprintf("%.15g", _tanh(-.1))));
  EXPECT_STREQ("0.46211715726001", gc(xasprintf("%.15g", _tanh(.5))));
  EXPECT_STREQ("-0.46211715726001", gc(xasprintf("%.15g", _tanh(-.5))));
  EXPECT_STREQ("0.761594155955765", gc(xasprintf("%.15g", _tanh(1.))));
  EXPECT_STREQ("-0.761594155955765", gc(xasprintf("%.15g", _tanh(-1.))));
  EXPECT_STREQ("0.905148253644866", gc(xasprintf("%.15g", _tanh(1.5))));
  EXPECT_STREQ("-0.905148253644866", gc(xasprintf("%.15g", _tanh(-1.5))));
  EXPECT_STREQ("0.964027580075817", gc(xasprintf("%.15g", _tanh(2.))));
  EXPECT_TRUE(isnan(_tanh(NAN)));
  EXPECT_TRUE(isnan(_tanh(-NAN)));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", _tanh(INFINITY))));
  EXPECT_STREQ("-1", gc(xasprintf("%.15g", _tanh(-INFINITY))));
  EXPECT_STREQ("2.2250738585072e-308",
               gc(xasprintf("%.15g", _tanh(__DBL_MIN__))));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", _tanh(__DBL_MAX__))));
  EXPECT_STREQ("-0.761594155955765",
               gc(xasprintf("%.15g", _tanh(-1.0000000000000002))));
  EXPECT_STREQ("-2.1073424255447e-08",
               gc(xasprintf("%.15g", _tanh(-2.1073424255447e-08))));
}

TEST(tanhf, test) {
  EXPECT_STREQ(".099668", gc(xdtoaf(_tanhf(+.1))));
  EXPECT_STREQ("-.099668", gc(xdtoaf(_tanhf(-.1))));
  EXPECT_STREQ("0", gc(xdtoaf(_tanhf(0))));
  EXPECT_STREQ("-0", gc(xdtoaf(_tanhf(-0.))));
  EXPECT_TRUE(isnan(_tanhf(NAN)));
  EXPECT_STREQ("1", gc(xdtoaf(_tanhf(INFINITY))));
  EXPECT_STREQ("-1", gc(xdtoaf(_tanhf(-INFINITY))));
}

TEST(tanhl, test) {
  EXPECT_STREQ(".09966799462495582", gc(xdtoal(_tanhl(+.1))));
  EXPECT_STREQ("-.09966799462495582", gc(xdtoal(_tanhl(-.1))));
  EXPECT_STREQ("0", gc(xdtoal(_tanhl(0))));
  EXPECT_STREQ("-0", gc(xdtoal(_tanhl(-0.))));
  EXPECT_TRUE(isnan(_tanhl(NAN)));
  EXPECT_STREQ("1", gc(xdtoal(_tanhl(INFINITY))));
  EXPECT_STREQ("-1", gc(xdtoal(_tanhl(-INFINITY))));
}

BENCH(tanhl, bench) {
  EZBENCH2("-tanh", donothing, _tanh(.7));    // ~27ns
  EZBENCH2("-tanhf", donothing, _tanhf(.7));  // ~15ns
  EZBENCH2("-tanhl", donothing, _tanhl(.7));  // ~42ns
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

#if 0
TEST(tanhf, brute) {
  long i;
  int lim = 100;
  uint32_t x, y;
  for (i = 0; i <= 0x100000000; i += 7) {
    x = f2i(tanhf(i2f(i)));
    y = f2i(tanhf2(i2f(i)));
    if (abs(x - y) > 2) {
      kprintf("bah %#lx %s %d\n", i, gc(xdtoaf(i2f(i))), abs(x - y));
      kprintf(" %-12s %#x\n", gc(xdtoaf(i2f(x))), x);
      kprintf(" %-12s %#x\n", gc(xdtoaf(i2f(y))), y);
      if (!--lim) break;
    }
  }
  if (lim != 100) {
    exit(1);
  }
}
#endif
