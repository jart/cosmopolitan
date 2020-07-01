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
#include "libc/runtime/gc.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

TEST(round, test) {
  EXPECT_STREQ("-3", gc(xdtoa(round(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(round(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(round(-.5))));
  EXPECT_STREQ("1", gc(xdtoa(round(.5))));
  EXPECT_STREQ("2", gc(xdtoa(round(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(round(2.5))));
}

TEST(round, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(round(-0.0))));
  EXPECT_STREQ("nan", gc(xdtoa(round(NAN))));
  EXPECT_STREQ("-nan", gc(xdtoa(round(-NAN))));
  EXPECT_STREQ("inf", gc(xdtoa(round(INFINITY))));
  EXPECT_STREQ("-inf", gc(xdtoa(round(-INFINITY))));
}

TEST(lround, test) {
  EXPECT_EQ(-3, lround(-2.5));
  EXPECT_EQ(-2, lround(-1.5));
  EXPECT_EQ(-1, lround(-.5));
  EXPECT_EQ(0, lround(-.0));
  EXPECT_EQ(1, lround(.5));
  EXPECT_EQ(2, lround(1.5));
  EXPECT_EQ(3, lround(2.5));
}

TEST(roundf, test) {
  EXPECT_STREQ("-3", gc(xdtoa(roundf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(roundf(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(roundf(-.5))));
  EXPECT_STREQ("1", gc(xdtoa(roundf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(roundf(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(roundf(2.5))));
}

TEST(roundf, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(roundf(-0.0))));
  EXPECT_STREQ("nan", gc(xdtoa(roundf(NAN))));
  EXPECT_STREQ("-nan", gc(xdtoa(roundf(-NAN))));
  EXPECT_STREQ("inf", gc(xdtoa(roundf(INFINITY))));
  EXPECT_STREQ("-inf", gc(xdtoa(roundf(-INFINITY))));
}

TEST(lroundf, test) {
  EXPECT_EQ(-3, lroundf(-2.5));
  EXPECT_EQ(-2, lroundf(-1.5));
  EXPECT_EQ(-1, lroundf(-.5));
  EXPECT_EQ(0, lroundf(-.0));
  EXPECT_EQ(1, lroundf(.5));
  EXPECT_EQ(2, lroundf(1.5));
  EXPECT_EQ(3, lroundf(2.5));
}
