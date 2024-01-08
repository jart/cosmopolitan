/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/mem/gc.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double _round(double) asm("round");
float _roundf(float) asm("roundf");
long double _roundl(long double) asm("roundl");

long _lround(double) asm("lround");
long _lroundf(float) asm("lroundf");
long _lroundl(long double) asm("lroundl");

long _lrint(double) asm("lrint");
long _lrintf(float) asm("lrintf");
long _lrintl(long double) asm("lrintl");

TEST(round, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(_round(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(_round(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(_round(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_round(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_round(-INFINITY))));
}

TEST(roundl, testCornerCases) {
  EXPECT_STREQ("-0", gc(xdtoa(_roundl(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(_roundl(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(_roundl(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_roundl(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_roundl(-INFINITY))));
}

TEST(round, test) {
  EXPECT_STREQ("-3", gc(xdtoa(_round(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(_round(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(_round(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(_round(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(_round(.4))));
  EXPECT_STREQ("1", gc(xdtoa(_round(.5))));
  EXPECT_STREQ("2", gc(xdtoa(_round(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(_round(2.5))));
}

TEST(roundf, test) {
  EXPECT_STREQ("-3", gc(xdtoa(_roundf(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(_roundf(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(_roundf(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(_roundf(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(_roundf(.4))));
  EXPECT_STREQ("1", gc(xdtoa(_roundf(.5))));
  EXPECT_STREQ("2", gc(xdtoa(_roundf(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(_roundf(2.5))));
}

TEST(roundl, test) {
  EXPECT_STREQ("-3", gc(xdtoa(_roundl(-2.5))));
  EXPECT_STREQ("-2", gc(xdtoa(_roundl(-1.5))));
  EXPECT_STREQ("-1", gc(xdtoa(_roundl(-.5))));
  EXPECT_STREQ("-0", gc(xdtoa(_roundl(-.4))));
  EXPECT_STREQ("0", gc(xdtoa(_roundl(.4))));
  EXPECT_STREQ("1", gc(xdtoa(_roundl(.5))));
  EXPECT_STREQ("2", gc(xdtoa(_roundl(1.5))));
  EXPECT_STREQ("3", gc(xdtoa(_roundl(2.5))));
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

TEST(lrint, test) {
  EXPECT_EQ(-2, _lrint(-2.5));
  EXPECT_EQ(-2, _lrint(-1.5));
  EXPECT_EQ(-0, _lrint(-.5));
  EXPECT_EQ(-0, _lrint(-.4));
  EXPECT_EQ(0, _lrint(.4));
  EXPECT_EQ(0, _lrint(.5));
  EXPECT_EQ(2, _lrint(1.5));
  EXPECT_EQ(2, _lrint(2.5));
}

TEST(lrintf, test) {
  EXPECT_EQ(-2, _lrintf(-2.5));
  EXPECT_EQ(-2, _lrintf(-1.5));
  EXPECT_EQ(-0, _lrintf(-.5));
  EXPECT_EQ(-0, _lrintf(-.4));
  EXPECT_EQ(0, _lrintf(.4));
  EXPECT_EQ(0, _lrintf(.5));
  EXPECT_EQ(2, _lrintf(1.5));
  EXPECT_EQ(2, _lrintf(2.5));
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
  EXPECT_STREQ("-0", gc(xdtoa(_roundf(-0.0))));
  EXPECT_STREQ("NAN", gc(xdtoa(_roundf(NAN))));
  EXPECT_STREQ("-NAN", gc(xdtoa(_roundf(-NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_roundf(INFINITY))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(_roundf(-INFINITY))));
}

TEST(lroundf, test) {
  EXPECT_EQ(-3, _lroundf(-2.5));
  EXPECT_EQ(-2, _lroundf(-1.5));
  EXPECT_EQ(-1, _lroundf(-.5));
  EXPECT_EQ(0, _lroundf(-.0));
  EXPECT_EQ(1, _lroundf(.5));
  EXPECT_EQ(2, _lroundf(1.5));
  EXPECT_EQ(3, _lroundf(2.5));
}

TEST(lround, test) {
  EXPECT_EQ(-3, _lround(-2.5));
  EXPECT_EQ(-2, _lround(-1.5));
  EXPECT_EQ(-1, _lround(-.5));
  EXPECT_EQ(-0, _lround(-.4));
  EXPECT_EQ(0, _lround(.4));
  EXPECT_EQ(1, _lround(.5));
  EXPECT_EQ(2, _lround(1.5));
  EXPECT_EQ(3, _lround(2.5));
}

TEST(lroundl, test) {
  EXPECT_EQ(-3, _lroundl(-2.5));
  EXPECT_EQ(-2, _lroundl(-1.5));
  EXPECT_EQ(-1, _lroundl(-.5));
  EXPECT_EQ(-0, _lroundl(-.4));
  EXPECT_EQ(0, _lroundl(.4));
  EXPECT_EQ(1, _lroundl(.5));
  EXPECT_EQ(2, _lroundl(1.5));
  EXPECT_EQ(3, _lroundl(2.5));
}

BENCH(round, bench) {
#ifdef __x86_64__
  EZBENCH2("double+.5", donothing,
           __expropriate(__veil("x", (double)(-3.5)) + .5));
  EZBENCH2("float+.5f", donothing,
           __expropriate(__veil("x", (float)(-3.5)) + .5));
  EZBENCH2("ldbl+.5l", donothing,
           __expropriate(__veil("t", (long double)(-3.5)) + .5));
#endif
  EZBENCH2("round", donothing, _round(.7));   /* ~4ns */
  EZBENCH2("roundf", donothing, _roundf(.7)); /* ~3ns */
  EZBENCH2("roundl", donothing, _roundl(.7)); /* ~8ns */
  EZBENCH2("lrint", donothing, _lrint(.7));   /* ~1ns */
  EZBENCH2("lrintf", donothing, _lrintf(.7)); /* ~1ns */
  EZBENCH2("lrintl", donothing, _lrintl(.7)); /* ~78ns */
}
