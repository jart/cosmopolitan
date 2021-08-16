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
#include "libc/runtime/gc.internal.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(tan, test) {
  EXPECT_STREQ("0", gc(xasprintf("%.15g", tan(0.))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", tan(-0.))));
  EXPECT_STREQ("0.54630248984379", gc(xasprintf("%.15g", tan(.5))));
  EXPECT_STREQ("-0.54630248984379", gc(xasprintf("%.15g", tan(-.5))));
  EXPECT_STREQ("1.5574077246549", gc(xasprintf("%.15g", tan(1.))));
  EXPECT_STREQ("-1.5574077246549", gc(xasprintf("%.15g", tan(-1.))));
  EXPECT_STREQ("14.1014199471717", gc(xasprintf("%.15g", tan(1.5))));
  EXPECT_STREQ("-14.1014199471717", gc(xasprintf("%.15g", tan(-1.5))));
  EXPECT_STREQ("nan", gc(xasprintf("%.15g", tan(NAN))));
  EXPECT_STREQ("-nan", gc(xasprintf("%.15g", tan(-NAN))));
  EXPECT_STREQ("-nan", gc(xasprintf("%.15g", tan(INFINITY))));
  EXPECT_STREQ("-nan", gc(xasprintf("%.15g", tan(-INFINITY))));
  EXPECT_STREQ("2.2250738585072e-308",
               gc(xasprintf("%.15g", tan(__DBL_MIN__))));
  EXPECT_STREQ("-0.0049620158744449", gc(xasprintf("%.15g", tan(__DBL_MAX__))));
}
