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
#include "libc/x/xasprintf.h"

TEST(cbrt, test) {
  EXPECT_STREQ("0", _gc(xasprintf("%.15g", cbrt(0.))));
  EXPECT_STREQ("-0", _gc(xasprintf("%.15g", cbrt(-0.))));
  EXPECT_STREQ("0.7937005259841", _gc(xasprintf("%.15g", cbrt(.5))));
  EXPECT_STREQ("-0.7937005259841", _gc(xasprintf("%.15g", cbrt(-.5))));
  EXPECT_STREQ("1", _gc(xasprintf("%.15g", cbrt(1.))));
  EXPECT_STREQ("-1", _gc(xasprintf("%.15g", cbrt(-1.))));
  EXPECT_STREQ("1.14471424255333", _gc(xasprintf("%.15g", cbrt(1.5))));
  EXPECT_STREQ("-1.14471424255333", _gc(xasprintf("%.15g", cbrt(-1.5))));
  EXPECT_STREQ("nan", _gc(xasprintf("%.15g", cbrt(NAN))));
  EXPECT_STREQ("-nan", _gc(xasprintf("%.15g", cbrt(-NAN))));
  EXPECT_STREQ("inf", _gc(xasprintf("%.15g", cbrt(INFINITY))));
  EXPECT_STREQ("-inf", _gc(xasprintf("%.15g", cbrt(-INFINITY))));
  EXPECT_STREQ("2.81264428523626e-103",
               _gc(xasprintf("%.15g", cbrt(__DBL_MIN__))));
  EXPECT_STREQ("5.64380309412236e+102",
               _gc(xasprintf("%.15g", cbrt(__DBL_MAX__))));
}
