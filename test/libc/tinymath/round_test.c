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
#include "libc/mem/gc.internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define roundl(x) roundl(VEIL("t", (long double)(x)))
#define round(x)  round(VEIL("x", (double)(x)))
#define roundf(x) roundf(VEIL("x", (float)(x)))

FIXTURE(intrin, disableHardwareExtensions) {
  memset((/*unconst*/ void *)kCpuids, 0, sizeof(kCpuids));
}

TEST(round, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(round(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(round(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(round(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(round(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(round(-INFINITY))));
}

TEST(roundl, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(roundl(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(roundl(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(roundl(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(roundl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(roundl(-INFINITY))));
}

TEST(round, test) {
  EXPECT_STREQ("-3", gc(xdtoa(round(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(round(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(round(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(round(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(round(.4))));
  EXPECT_STREQ("1", gc(xdtoa(round(.5))));
  EXPECT_STREQ("2", gc(xdtoa(round(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(round(2.5))));
}

TEST(roundf, test) {
  EXPECT_STREQ("-3", gc(xdtoa(roundf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(roundf(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(roundf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(roundf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(roundf(.4))));
  EXPECT_STREQ("1", gc(xdtoa(roundf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(roundf(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(roundf(2.5))));
}

TEST(roundl, test) {
  EXPECT_STREQ("-3", gc(xdtoa(roundl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(roundl(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(roundl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(roundl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(roundl(.4))));
  EXPECT_STREQ("1", gc(xdtoa(roundl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(roundl(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(roundl(2.5))));
}

TEST(nearbyint, test) {
  EXPECT_STREQ("-2", gc(xdtoa(nearbyint(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(nearbyint(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(nearbyint(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(nearbyint(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(nearbyint(.4))));
  EXPECT_STREQ("0", gc(xdtoa(nearbyint(.5))));
  EXPECT_STREQ("2", gc(xdtoa(nearbyint(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(nearbyint(2.5))));
}

TEST(nearbyintf, test) {
  EXPECT_STREQ("-2", gc(xdtoa(nearbyintf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(nearbyintf(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(nearbyintf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(nearbyintf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(nearbyintf(.4))));
  EXPECT_STREQ("0", gc(xdtoa(nearbyintf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(nearbyintf(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(nearbyintf(2.5))));
}

TEST(nearbyintl, test) {
  EXPECT_STREQ("-2", gc(xdtoa(nearbyintl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(nearbyintl(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(nearbyintl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(nearbyintl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(nearbyintl(.4))));
  EXPECT_STREQ("0", gc(xdtoa(nearbyintl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(nearbyintl(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(nearbyintl(2.5))));
}

TEST(rint, test) {
  EXPECT_STREQ("-2", gc(xdtoa(rint(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(rint(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(rint(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(rint(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(rint(.4))));
  EXPECT_STREQ("0", gc(xdtoa(rint(.5))));
  EXPECT_STREQ("2", gc(xdtoa(rint(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(rint(2.5))));
}

TEST(rintf, test) {
  EXPECT_STREQ("-2", gc(xdtoa(rintf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(rintf(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(rintf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(rintf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(rintf(.4))));
  EXPECT_STREQ("0", gc(xdtoa(rintf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(rintf(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(rintf(2.5))));
}

TEST(rintl, test) {
  EXPECT_STREQ("-2", gc(xdtoa(rintl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(rintl(-1.5))));
  EXPECT_STREQ("-0", gc(xdtoa(rintl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(rintl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(rintl(.4))));
  EXPECT_STREQ("0", gc(xdtoa(rintl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(rintl(1.5))));
  EXPECT_STREQ("2", gc(xdtoa(rintl(2.5))));
}

TEST(roundf, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(roundf(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(roundf(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(roundf(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(roundf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(roundf(-INFINITY))));
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

TEST(lround, test) {
  EXPECT_EQ(-3, lround(-2.5));
  EXPECT_EQ(-2, lround(-1.5));
  EXPECT_EQ(-1, lround(-.5));
  EXPECT_EQ(-0, lround(-.4));
  EXPECT_EQ(0, lround(.4));
  EXPECT_EQ(1, lround(.5));
  EXPECT_EQ(2, lround(1.5));
  EXPECT_EQ(3, lround(2.5));
}

TEST(lroundl, test) {
  EXPECT_EQ(-3, lroundl(-2.5));
  EXPECT_EQ(-2, lroundl(-1.5));
  EXPECT_EQ(-1, lroundl(-.5));
  EXPECT_EQ(-0, lroundl(-.4));
  EXPECT_EQ(0, lroundl(.4));
  EXPECT_EQ(1, lroundl(.5));
  EXPECT_EQ(2, lroundl(1.5));
  EXPECT_EQ(3, lroundl(2.5));
}
