/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/math.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/tinymath.h"
#include "libc/x/x.h"

float tinymath_roundf$k8(float);
double tinymath_round$k8(double);

TEST(round, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_round(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_round(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_round(-.5))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_round(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_round(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_round(2.5))));
}

TEST(round, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_round(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(tinymath_round(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(tinymath_round(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(tinymath_round(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(tinymath_round(-INFINITY))));
}

TEST(lround, test) {
  EXPECT_EQ(-3, tinymath_lround(-2.5));
  EXPECT_EQ(-2, tinymath_lround(-1.5));
  EXPECT_EQ(-1, tinymath_lround(-.5));
  EXPECT_EQ(0, tinymath_lround(-.0));
  EXPECT_EQ(1, tinymath_lround(.5));
  EXPECT_EQ(2, tinymath_lround(1.5));
  EXPECT_EQ(3, tinymath_lround(2.5));
}

TEST(roundf, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_roundf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_roundf(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_roundf(-.5))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_roundf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_roundf(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_roundf(2.5))));
}

TEST(roundf, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_roundf(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(tinymath_roundf(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(tinymath_roundf(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(tinymath_roundf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(tinymath_roundf(-INFINITY))));
}

TEST(lroundf, test) {
  EXPECT_EQ(-3, tinymath_lroundf(-2.5));
  EXPECT_EQ(-2, tinymath_lroundf(-1.5));
  EXPECT_EQ(-1, tinymath_lroundf(-.5));
  EXPECT_EQ(0, tinymath_lroundf(-.0));
  EXPECT_EQ(1, tinymath_lroundf(.5));
  EXPECT_EQ(2, tinymath_lroundf(1.5));
  EXPECT_EQ(3, tinymath_lroundf(2.5));
}

#if !X86_NEED(SSE4_2)

TEST(round$k8, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_round$k8(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_round$k8(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_round$k8(-.5))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_round$k8(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_round$k8(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_round$k8(2.5))));
}

TEST(roundf$k8, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_roundf$k8(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_roundf$k8(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_roundf$k8(-.5))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_roundf$k8(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_roundf$k8(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_roundf$k8(2.5))));
}

TEST(round$k8, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_round$k8(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(tinymath_round$k8(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(tinymath_round$k8(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(tinymath_round$k8(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(tinymath_round$k8(-INFINITY))));
}

TEST(roundf$k8, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_roundf$k8(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(tinymath_roundf$k8(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(tinymath_roundf$k8(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(tinymath_roundf$k8(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(tinymath_roundf$k8(-INFINITY))));
}

#endif
