/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/tinymath.h"
#include "libc/x/x.h"

float tinymath_roundf$k8(float);
double tinymath_round$k8(double);

FIXTURE(intrin, disableHardwareExtensions) {
  memset((/*unconst*/ void *)kCpuids, 0, sizeof(kCpuids));
}

TEST(round, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_round(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(tinymath_round(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(tinymath_round(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(tinymath_round(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(tinymath_round(-INFINITY))));
}

TEST(roundl, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_roundl(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(tinymath_roundl(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(tinymath_roundl(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(tinymath_roundl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(tinymath_roundl(-INFINITY))));
}

TEST(round, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_round(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_round(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_round(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_round(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_round(.4))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_round(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_round(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_round(2.5))));
}

TEST(roundf, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_roundf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_roundf(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_roundf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_roundf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_roundf(.4))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_roundf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_roundf(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_roundf(2.5))));
}

TEST(roundl, test) {
  EXPECT_STREQ("-3", gc(xdtoa(tinymath_roundl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_roundl(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(tinymath_roundl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_roundl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_roundl(.4))));
  EXPECT_STREQ("1", gc(xdtoa(tinymath_roundl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_roundl(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(tinymath_roundl(2.5))));
}

TEST(nearbyint, test) {
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_nearbyint(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_nearbyint(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_nearbyint(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_nearbyint(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_nearbyint(.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_nearbyint(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_nearbyint(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_nearbyint(2.5))));
}

TEST(nearbyintf, test) {
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_nearbyintf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_nearbyintf(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_nearbyintf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_nearbyintf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_nearbyintf(.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_nearbyintf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_nearbyintf(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_nearbyintf(2.5))));
}

TEST(nearbyintl, test) {
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_nearbyintl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_nearbyintl(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_nearbyintl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_nearbyintl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_nearbyintl(.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_nearbyintl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_nearbyintl(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_nearbyintl(2.5))));
}

TEST(rint, test) {
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_rint(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_rint(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_rint(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_rint(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_rint(.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_rint(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_rint(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_rint(2.5))));
}

TEST(rintf, test) {
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_rintf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_rintf(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_rintf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_rintf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_rintf(.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_rintf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_rintf(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_rintf(2.5))));
}

TEST(rintl, test) {
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_rintl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(tinymath_rintl(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_rintl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(tinymath_rintl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_rintl(.4))));
  EXPECT_STREQ("0", gc(xdtoa(tinymath_rintl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_rintl(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(tinymath_rintl(2.5))));
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

TEST(lround, test) {
  EXPECT_EQ(-3, tinymath_lround(-2.5));
  EXPECT_EQ(-2, tinymath_lround(-1.5));
  EXPECT_EQ(-1, tinymath_lround(-.5));
  EXPECT_EQ(-0, tinymath_lround(-.4));
  EXPECT_EQ(0, tinymath_lround(.4));
  EXPECT_EQ(1, tinymath_lround(.5));
  EXPECT_EQ(2, tinymath_lround(1.5));
  EXPECT_EQ(3, tinymath_lround(2.5));
}

TEST(lroundl, test) {
  EXPECT_EQ(-3, tinymath_lroundl(-2.5));
  EXPECT_EQ(-2, tinymath_lroundl(-1.5));
  EXPECT_EQ(-1, tinymath_lroundl(-.5));
  EXPECT_EQ(-0, tinymath_lroundl(-.4));
  EXPECT_EQ(0, tinymath_lroundl(.4));
  EXPECT_EQ(1, tinymath_lroundl(.5));
  EXPECT_EQ(2, tinymath_lroundl(1.5));
  EXPECT_EQ(3, tinymath_lroundl(2.5));
}
