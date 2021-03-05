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
#include "libc/fmt/fmt.h"
#include "libc/math.h"
#include "libc/runtime/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(RealFormatting, g) {
  EXPECT_STREQ("nan", gc(xasprintf("%g", NAN)));
  EXPECT_STREQ("-nan", gc(xasprintf("%g", -NAN)));
  EXPECT_STREQ("inf", gc(xasprintf("%g", INFINITY)));
  EXPECT_STREQ("-inf", gc(xasprintf("%g", -INFINITY)));
  EXPECT_STREQ("0", gc(xasprintf("%g", 0.)));
  EXPECT_STREQ("-0", gc(xasprintf("%g", -0.)));
  EXPECT_STREQ("1", gc(xasprintf("%g", 1.)));
  EXPECT_STREQ("-1", gc(xasprintf("%g", -1.)));
  EXPECT_STREQ("10", gc(xasprintf("%g", 10.)));
  EXPECT_STREQ("10", gc(xasprintf("%.0g", 10.)));
  EXPECT_STREQ("-10", gc(xasprintf("%g", -10.)));
  EXPECT_STREQ("-10", gc(xasprintf("%.0g", -10.)));
  EXPECT_STREQ("       -10", gc(xasprintf("%10g", -10.)));
  EXPECT_STREQ("       -10", gc(xasprintf("%*g", 10, -10.)));
  EXPECT_STREQ("1e+100", gc(xasprintf("%g", 1e100)));
  EXPECT_STREQ("1e-100", gc(xasprintf("%g", 1e-100)));
  EXPECT_STREQ("-1e-100", gc(xasprintf("%g", -1e-100)));
  EXPECT_STREQ("3.14159", gc(xasprintf("%g", 0x1.921fb54442d1846ap+1)));
}

TEST(RealFormatting, f) {
  EXPECT_STREQ("3.141593", gc(xasprintf("%f", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("3.1415926535897931",
               gc(xasprintf("%.16f", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("100000000000000001590289110975991804683608085639452813"
               "89781327557747838772170381060813469985856815104.000000",
               gc(xasprintf("%f", 1e100)));
}

TEST(RealFormatting, e) {
  EXPECT_STREQ("3.14159", gc(xasprintf("%g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("3.141592653589793",
               gc(xasprintf("%.16g", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("1.000000e+100", gc(xasprintf("%e", 1e100)));
  EXPECT_STREQ("1.000000E+100", gc(xasprintf("%E", 1e100)));
}

TEST(RealFormatting, a) {
  EXPECT_STREQ("0x1.921fb54442d18p+1",
               gc(xasprintf("%a", 0x1.921fb54442d1846ap+1)));
  EXPECT_STREQ("0X1.921FB54442D18P+1",
               gc(xasprintf("%A", 0x1.921fb54442d1846ap+1)));
}
