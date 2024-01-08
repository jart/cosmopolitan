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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"

TEST(tgamma, test) {
  EXPECT_STREQ("inf", gc(xasprintf("%.15g", tgamma(0.))));
  EXPECT_STREQ("-inf", gc(xasprintf("%.15g", tgamma(-0.))));
  EXPECT_STREQ("9.51350769866873", gc(xasprintf("%.15g", tgamma(.1))));
  EXPECT_STREQ("-10.6862870211932", gc(xasprintf("%.15g", tgamma(-.1))));
  EXPECT_STREQ("1.77245385090552", gc(xasprintf("%.15g", tgamma(.5))));
  EXPECT_STREQ("-3.54490770181103", gc(xasprintf("%.15g", tgamma(-.5))));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", tgamma(1.))));
  EXPECT_TRUE(isnan(tgamma(-1.)));
  EXPECT_STREQ("0.886226925452758", gc(xasprintf("%.15g", tgamma(1.5))));
  EXPECT_STREQ("2.36327180120735", gc(xasprintf("%.15g", tgamma(-1.5))));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", tgamma(2.))));
  EXPECT_TRUE(isnan(tgamma(NAN)));
  EXPECT_TRUE(isnan(tgamma(-NAN)));
  EXPECT_STREQ("inf", gc(xasprintf("%.15g", tgamma(INFINITY))));
  EXPECT_TRUE(isnan(tgamma(-INFINITY)));
  EXPECT_STREQ("4.49423283715579e+307",
               gc(xasprintf("%.15g", tgamma(__DBL_MIN__))));
  EXPECT_STREQ("inf", gc(xasprintf("%.15g", tgamma(__DBL_MAX__))));
  EXPECT_STREQ("4.5035996273705e+15",
               gc(xasprintf("%.14g", tgamma(-1.0000000000000002))));
  EXPECT_STREQ("-47453133.3893415",
               gc(xasprintf("%.15g", tgamma(-2.1073424255447e-08))));
}
