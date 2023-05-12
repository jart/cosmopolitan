/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

double _tanh(double) asm("tanh");
float _tanhf(float) asm("tanhf");
long double _tanhl(long double) asm("tanhl");

TEST(_tanhl, test) {
  EXPECT_STREQ(".09966799462495582", _gc(xdtoal(_tanhl(+.1))));
  EXPECT_STREQ("-.09966799462495582", _gc(xdtoal(_tanhl(-.1))));
  EXPECT_STREQ("0", _gc(xdtoal(_tanhl(0))));
  EXPECT_STREQ("-0", _gc(xdtoal(_tanhl(-0.))));
  EXPECT_TRUE(isnan(_tanhl(NAN)));
  EXPECT_STREQ("1", _gc(xdtoal(_tanhl(INFINITY))));
  EXPECT_STREQ("-1", _gc(xdtoal(_tanhl(-INFINITY))));
}

TEST(_tanh, test) {
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", _tanh(0.))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", _tanh(-0.))));
  EXPECT_STREQ("0.0996679946249558", _gc(xasprintf("%.15g", _tanh(.1))));
  EXPECT_STREQ("-0.0996679946249558", _gc(xasprintf("%.15g", _tanh(-.1))));
  EXPECT_STREQ("0.46211715726001", _gc(xasprintf("%.15g", _tanh(.5))));
  EXPECT_STREQ("-0.46211715726001", _gc(xasprintf("%.15g", _tanh(-.5))));
  EXPECT_STREQ("0.761594155955765", _gc(xasprintf("%.15g", _tanh(1.))));
  EXPECT_STREQ("-0.761594155955765", _gc(xasprintf("%.15g", _tanh(-1.))));
  EXPECT_STREQ("0.905148253644866", _gc(xasprintf("%.15g", _tanh(1.5))));
  EXPECT_STREQ("-0.905148253644866", _gc(xasprintf("%.15g", _tanh(-1.5))));
  EXPECT_STREQ("0.964027580075817", _gc(xasprintf("%.15g", _tanh(2.))));
  EXPECT_TRUE(isnan(_tanh(NAN)));
  EXPECT_TRUE(isnan(_tanh(-NAN)));
  EXPECT_STREQ("1", _gc(xasprintf("%.15g", _tanh(INFINITY))));
  EXPECT_STREQ("-1", _gc(xasprintf("%.15g", _tanh(-INFINITY))));
  EXPECT_STREQ("2.2250738585072e-308",
               _gc(xasprintf("%.15g", _tanh(__DBL_MIN__))));
  EXPECT_STREQ("1", _gc(xasprintf("%.15g", _tanh(__DBL_MAX__))));
  EXPECT_STREQ("-0.761594155955765",
               _gc(xasprintf("%.15g", _tanh(-1.0000000000000002))));
  EXPECT_STREQ("-2.1073424255447e-08",
               _gc(xasprintf("%.15g", _tanh(-2.1073424255447e-08))));
}

TEST(_tanhf, test) {
  EXPECT_STREQ(".099668", _gc(xdtoaf(_tanhf(+.1))));
  EXPECT_STREQ("-.099668", _gc(xdtoaf(_tanhf(-.1))));
  EXPECT_STREQ("0", _gc(xdtoaf(_tanhf(0))));
  EXPECT_STREQ("-0", _gc(xdtoaf(_tanhf(-0.))));
  EXPECT_TRUE(isnan(_tanhf(NAN)));
  EXPECT_STREQ("1", _gc(xdtoaf(_tanhf(INFINITY))));
  EXPECT_STREQ("-1", _gc(xdtoaf(_tanhf(-INFINITY))));
}
