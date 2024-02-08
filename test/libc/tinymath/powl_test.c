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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/errno.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/runtime/pc.internal.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "libc/x/xasprintf.h"

int rando;
char buf[128];

void SetUp(void) {
  rando = rand() & 0xffff;
}

static double POW(double x, double y) {
  return powl(x, y);
}

static char *fmtd(double x) {
  sprintf(buf, "%.15g", x);
  return buf;
}

static char *fmtf(float x) {
  sprintf(buf, "%.6g", x);
  return buf;
}

TEST(powl, test) {
  EXPECT_STREQ("27", gc(xdtoal(powl(3, 3))));
  EXPECT_STREQ("-27", gc(xdtoal(powl(-3, 3))));
  EXPECT_STREQ("1e+4932", gc(xdtoal(powl(10, 4932))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(10, 4933))));
  // EXPECT_STREQ("0", gc(xdtoal(powl(10, -5000))));
  EXPECT_STREQ("1.063382396627933e+37", gc(xdtoal(powl(2, 123))));
  EXPECT_STARTSWITH(".4248496805467504", gc(xdtoal(powl(.7, 2.4))));
  EXPECT_STREQ("1", gc(xdtoal(powl(1, NAN))));
  EXPECT_STREQ("1", gc(xdtoal(powl(1, rando))));
  EXPECT_STREQ("1", gc(xdtoal(powl(NAN, 0))));
  EXPECT_STREQ("1", gc(xdtoal(powl(rando, 0))));
  EXPECT_STREQ("0", gc(xdtoal(powl(0, 1))));
  EXPECT_STREQ("0", gc(xdtoal(powl(0, 2))));
  EXPECT_STREQ("0", gc(xdtoal(powl(0, 2.1))));
  EXPECT_STREQ("1", gc(xdtoal(powl(-1, INFINITY))));
  EXPECT_STREQ("1", gc(xdtoal(powl(-1, -INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(1. / MAX(2, rando), -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoal(powl(1.1, -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoal(powl(MAX(2, rando), -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoal(powl(1. / MAX(2, rando), INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(MAX(2, rando), INFINITY))));
  EXPECT_STREQ("-0", gc(xdtoal(powl(-INFINITY, -1))));
  EXPECT_STREQ("0", gc(xdtoal(powl(-INFINITY, -1.1))));
  EXPECT_STREQ("0", gc(xdtoal(powl(-INFINITY, -2))));
  EXPECT_STREQ("0", gc(xdtoal(powl(-INFINITY, -2.1))));
  EXPECT_STREQ("-0", gc(xdtoal(powl(-INFINITY, -3))));
  EXPECT_STREQ("0", gc(xdtoal(powl(-INFINITY, -3.1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(powl(-INFINITY, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(-INFINITY, 1.1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(-INFINITY, 2))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(-INFINITY, 2.1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(powl(-INFINITY, 3))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(-INFINITY, 3.1))));
  EXPECT_STREQ("0", gc(xdtoal(powl(INFINITY, -1))));
  EXPECT_STREQ("0", gc(xdtoal(powl(INFINITY, -.1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(INFINITY, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(INFINITY, .1))));
  EXPECT_STREQ("1", gc(xdtoal(powl(INFINITY, 0))));
  EXPECT_STREQ("1", gc(xdtoal(powl(INFINITY, -0.))));
  EXPECT_STREQ("1", gc(xdtoal(powl(0, 0))));
  EXPECT_STREQ("1", gc(xdtoal(powl(0, -0.))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(0, -(MAX(rando, 1) | 1)))));
  EXPECT_STREQ("-INFINITY", gc(xdtoal(powl(-0., -(MAX(rando, 1) | 1)))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(0, -(rando & -2)))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(-0., -(rando & -2)))));
  EXPECT_TRUE(isnan(powl(-3, 1. / MAX(rando, 2))));
  EXPECT_TRUE(isnan(powl(-3, -(1. / MAX(rando, 2)))));
  EXPECT_STREQ("-.3333333333333333", gc(xdtoal(powl(-3, -1))));
  EXPECT_STREQ(".1111111111111111", gc(xdtoal(powl(-3, -2))));
  EXPECT_STREQ("-0", gc(xdtoal(powl(-0., MAX(1, rando) | 1))));
  EXPECT_STREQ("0", gc(xdtoal(powl(-0., MAX(1, rando) & ~1))));
}

TEST(pow, test) {
  EXPECT_STREQ("27", fmtd(pow(3, 3)));
  EXPECT_STREQ("-27", fmtd(pow(-3, 3)));
  EXPECT_STREQ("1e+308", fmtd(pow(10, 308)));
  EXPECT_STREQ("inf", fmtd(pow(10, 309)));
  EXPECT_STREQ("0", fmtd(pow(10, -5000)));
  EXPECT_STREQ("1.06338239662793e+37", fmtd(pow(2, 123)));
  EXPECT_STARTSWITH("0.42484968054675", fmtd(pow(.7, 2.4)));
  EXPECT_STREQ("1", fmtd(pow(1, NAN)));
  EXPECT_STREQ("1", fmtd(pow(1, rando)));
  EXPECT_STREQ("1", fmtd(pow(NAN, 0)));
  EXPECT_STREQ("1", fmtd(pow(rando, 0)));
  EXPECT_STREQ("0", fmtd(pow(0, 1)));
  EXPECT_STREQ("0", fmtd(pow(0, 2)));
  EXPECT_STREQ("0", fmtd(pow(0, 2.1)));
  EXPECT_STREQ("1", fmtd(pow(-1, INFINITY)));
  EXPECT_STREQ("1", fmtd(pow(-1, -INFINITY)));
  EXPECT_STREQ("inf", fmtd(pow(1. / MAX(2, rando), -INFINITY)));
  EXPECT_STREQ("0", fmtd(pow(1.1, -INFINITY)));
  EXPECT_STREQ("0", fmtd(pow(MAX(2, rando), -INFINITY)));
  EXPECT_STREQ("0", fmtd(pow(1. / MAX(2, rando), INFINITY)));
  EXPECT_STREQ("inf", fmtd(pow(MAX(2, rando), INFINITY)));
  EXPECT_STREQ("-0", fmtd(pow(-INFINITY, -1)));
  EXPECT_STREQ("0", fmtd(pow(-INFINITY, -1.1)));
  EXPECT_STREQ("0", fmtd(pow(-INFINITY, -2)));
  EXPECT_STREQ("0", fmtd(pow(-INFINITY, -2.1)));
  EXPECT_STREQ("-0", fmtd(pow(-INFINITY, -3)));
  EXPECT_STREQ("0", fmtd(pow(-INFINITY, -3.1)));
  EXPECT_STREQ("-inf", fmtd(pow(-INFINITY, 1)));
  EXPECT_STREQ("inf", fmtd(pow(-INFINITY, 1.1)));
  EXPECT_STREQ("inf", fmtd(pow(-INFINITY, 2)));
  EXPECT_STREQ("inf", fmtd(pow(-INFINITY, 2.1)));
  EXPECT_STREQ("-inf", fmtd(pow(-INFINITY, 3)));
  EXPECT_STREQ("inf", fmtd(pow(-INFINITY, 3.1)));
  EXPECT_STREQ("0", fmtd(pow(INFINITY, -1)));
  EXPECT_STREQ("0", fmtd(pow(INFINITY, -.1)));
  EXPECT_STREQ("inf", fmtd(pow(INFINITY, 1)));
  EXPECT_STREQ("inf", fmtd(pow(INFINITY, .1)));
  EXPECT_STREQ("1", fmtd(pow(INFINITY, 0)));
  EXPECT_STREQ("1", fmtd(pow(INFINITY, -0.)));
  EXPECT_STREQ("1", fmtd(pow(0, 0)));
  EXPECT_STREQ("1", fmtd(pow(0, -0.)));
  EXPECT_STREQ("inf", fmtd(pow(0, -(MAX(rando, 1) | 1))));
  EXPECT_STREQ("-inf", fmtd(pow(-0., -(MAX(rando, 1) | 1))));
  EXPECT_STREQ("inf", fmtd(pow(0, -(rando & -2))));
  EXPECT_STREQ("inf", fmtd(pow(-0., -(rando & -2))));
  EXPECT_STREQ("-0.333333333333333", fmtd(pow(-3, -1)));
  EXPECT_STREQ("0.111111111111111", fmtd(pow(-3, -2)));
  EXPECT_STREQ("-0", gc(xdtoa(pow(-0., MAX(1, rando) | 1))));
  EXPECT_STREQ("0", gc(xdtoa(pow(-0., MAX(1, rando) & ~1))));
}

TEST(powf, test) {
  EXPECT_STREQ("27", fmtf(powf(3, 3)));
  EXPECT_STREQ("-27", fmtf(powf(-3, 3)));
  EXPECT_STREQ("1e+38", fmtf(powf(10, 38)));
  EXPECT_STREQ("inf", fmtf(powf(10, 39)));
  EXPECT_STREQ("0", fmtf(powf(10, -5000)));
  EXPECT_STREQ("1.06338e+37", fmtf(powf(2, 123)));
  EXPECT_STARTSWITH("0.42485", fmtf(powf(.7, 2.4)));
  EXPECT_STREQ("1", fmtf(powf(1, NAN)));
  EXPECT_STREQ("1", fmtf(powf(1, rando)));
  EXPECT_STREQ("1", fmtf(powf(NAN, 0)));
  EXPECT_STREQ("1", fmtf(powf(rando, 0)));
  EXPECT_STREQ("0", fmtf(powf(0, 1)));
  EXPECT_STREQ("0", fmtf(powf(0, 2)));
  EXPECT_STREQ("0", fmtf(powf(0, 2.1)));
  EXPECT_STREQ("1", fmtf(powf(-1, INFINITY)));
  EXPECT_STREQ("1", fmtf(powf(-1, -INFINITY)));
  EXPECT_STREQ("inf", fmtf(powf(1. / MAX(2, rando), -INFINITY)));
  EXPECT_STREQ("0", fmtf(powf(1.1, -INFINITY)));
  EXPECT_STREQ("0", fmtf(powf(MAX(2, rando), -INFINITY)));
  EXPECT_STREQ("0", fmtf(powf(1. / MAX(2, rando), INFINITY)));
  EXPECT_STREQ("inf", fmtf(powf(MAX(2, rando), INFINITY)));
  EXPECT_STREQ("-0", fmtf(powf(-INFINITY, -1)));
  EXPECT_STREQ("0", fmtf(powf(-INFINITY, -1.1)));
  EXPECT_STREQ("0", fmtf(powf(-INFINITY, -2)));
  EXPECT_STREQ("0", fmtf(powf(-INFINITY, -2.1)));
  EXPECT_STREQ("-0", fmtf(powf(-INFINITY, -3)));
  EXPECT_STREQ("0", fmtf(powf(-INFINITY, -3.1)));
  EXPECT_STREQ("-inf", fmtf(powf(-INFINITY, 1)));
  EXPECT_STREQ("inf", fmtf(powf(-INFINITY, 1.1)));
  EXPECT_STREQ("inf", fmtf(powf(-INFINITY, 2)));
  EXPECT_STREQ("inf", fmtf(powf(-INFINITY, 2.1)));
  EXPECT_STREQ("-inf", fmtf(powf(-INFINITY, 3)));
  EXPECT_STREQ("inf", fmtf(powf(-INFINITY, 3.1)));
  EXPECT_STREQ("0", fmtf(powf(INFINITY, -1)));
  EXPECT_STREQ("0", fmtf(powf(INFINITY, -.1)));
  EXPECT_STREQ("inf", fmtf(powf(INFINITY, 1)));
  EXPECT_STREQ("inf", fmtf(powf(INFINITY, .1)));
  EXPECT_STREQ("1", fmtf(powf(INFINITY, 0)));
  EXPECT_STREQ("1", fmtf(powf(INFINITY, -0.)));
  EXPECT_STREQ("1", fmtf(powf(0, 0)));
  EXPECT_STREQ("1", fmtf(powf(0, -0.)));
  EXPECT_STREQ("inf", fmtf(powf(0, -(MAX(rando, 1) | 1))));
  EXPECT_STREQ("-inf", fmtf(powf(-0., -(MAX(rando, 1) | 1))));
  EXPECT_STREQ("inf", fmtf(powf(0, -(rando & -2))));
  EXPECT_STREQ("inf", fmtf(powf(-0., -(rando & -2))));
  EXPECT_STREQ("-0.333333", fmtf(powf(-3, -1)));
  EXPECT_STREQ("0.111111", fmtf(powf(-3, -2)));
  EXPECT_STREQ("-0", gc(xdtoaf(powf(-0., MAX(1, rando) | 1))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(-0., MAX(1, rando) & ~1))));
}

#if 0
TEST(powl, errors) {
  EXPECT_STREQ("1", fmtd(POW(0., 0.)));
  EXPECT_STREQ("1", fmtd(POW(0., -0.)));
  EXPECT_STREQ("0", fmtd(POW(0., .5)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(0., -.5)));
  EXPECT_EQ(ERANGE, errno);
  EXPECT_STREQ("0", fmtd(POW(0., 1.)));
  EXPECT_STREQ("inf", fmtd(POW(0., -1.)));
  EXPECT_STREQ("0", fmtd(POW(0., 1.5)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(0., -1.5)));
  EXPECT_EQ(ERANGE, errno);
  EXPECT_STREQ("0", fmtd(POW(0., 2.)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(0., -2.)));
  EXPECT_EQ(ERANGE, errno);
  EXPECT_TRUE(isnan(POW(0., NAN)));
  EXPECT_TRUE(isnan(POW(0., -NAN)));
  EXPECT_STREQ("0", fmtd(POW(0., INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(0., -INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(0., __DBL_MIN__)));
  EXPECT_STREQ("0", fmtd(POW(0., __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(-0., 0.)));
  EXPECT_STREQ("1", fmtd(POW(-0., -0.)));
  EXPECT_STREQ("0", fmtd(POW(-0., .5)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(-0., -.5)));
  EXPECT_EQ(ERANGE, errno);
  EXPECT_STREQ("-0", fmtd(POW(-0., 1.)));
  EXPECT_STREQ("-inf", fmtd(POW(-0., -1.)));
  EXPECT_STREQ("0", fmtd(POW(-0., 1.5)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(-0., -1.5)));
  EXPECT_EQ(ERANGE, errno);
  EXPECT_STREQ("0", fmtd(POW(-0., 2.)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(-0., -2.)));
  EXPECT_EQ(ERANGE, errno);
  EXPECT_TRUE(isnan(POW(-0., NAN)));
  EXPECT_TRUE(isnan(POW(-0., -NAN)));
  EXPECT_STREQ("0", fmtd(POW(-0., INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(-0., -INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(-0., __DBL_MIN__)));
  EXPECT_STREQ("0", fmtd(POW(-0., __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(.5, 0.)));
  EXPECT_STREQ("1", fmtd(POW(.5, -0.)));
  EXPECT_STREQ("0.707106781186548", fmtd(POW(.5, .5)));
  EXPECT_STREQ("1.4142135623731", fmtd(POW(.5, -.5)));
  EXPECT_STREQ("0.5", fmtd(POW(.5, 1.)));
  EXPECT_STREQ("2", fmtd(POW(.5, -1.)));
  EXPECT_STREQ("0.353553390593274", fmtd(POW(.5, 1.5)));
  EXPECT_STREQ("2.82842712474619", fmtd(POW(.5, -1.5)));
  EXPECT_STREQ("0.25", fmtd(POW(.5, 2.)));
  EXPECT_STREQ("4", fmtd(POW(.5, -2.)));
  EXPECT_TRUE(isnan(POW(.5, NAN)));
  EXPECT_TRUE(isnan(POW(.5, -NAN)));
  EXPECT_STREQ("0", fmtd(POW(.5, INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(.5, -INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(.5, __DBL_MIN__)));
  errno = 0;
  EXPECT_STREQ("0", fmtd(POW(.5, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(-.5, 0.)));
  EXPECT_STREQ("1", fmtd(POW(-.5, -0.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-.5, .5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-.5, -.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("-0.5", fmtd(POW(-.5, 1.)));
  EXPECT_STREQ("-2", fmtd(POW(-.5, -1.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-.5, 1.5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-.5, -1.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("0.25", fmtd(POW(-.5, 2.)));
  EXPECT_STREQ("4", fmtd(POW(-.5, -2.)));
  EXPECT_TRUE(isnan(POW(-.5, NAN)));
  EXPECT_TRUE(isnan(POW(-.5, -NAN)));
  EXPECT_STREQ("0", fmtd(POW(-.5, INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(-.5, -INFINITY)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-.5, __DBL_MIN__)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_STREQ("0", fmtd(POW(-.5, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(1., 0.)));
  EXPECT_STREQ("1", fmtd(POW(1., -0.)));
  EXPECT_STREQ("1", fmtd(POW(1., .5)));
  EXPECT_STREQ("1", fmtd(POW(1., -.5)));
  EXPECT_STREQ("1", fmtd(POW(1., 1.)));
  EXPECT_STREQ("1", fmtd(POW(1., -1.)));
  EXPECT_STREQ("1", fmtd(POW(1., 1.5)));
  EXPECT_STREQ("1", fmtd(POW(1., -1.5)));
  EXPECT_STREQ("1", fmtd(POW(1., 2.)));
  EXPECT_STREQ("1", fmtd(POW(1., -2.)));
  EXPECT_STREQ("1", fmtd(POW(1., NAN)));
  EXPECT_STREQ("1", fmtd(POW(1., -NAN)));
  EXPECT_STREQ("1", fmtd(POW(1., INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(1., -INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(1., __DBL_MIN__)));
  EXPECT_STREQ("1", fmtd(POW(1., __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(-1., 0.)));
  EXPECT_STREQ("1", fmtd(POW(-1., -0.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1., .5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1., -.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("-1", fmtd(POW(-1., 1.)));
  EXPECT_STREQ("-1", fmtd(POW(-1., -1.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1., 1.5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1., -1.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("1", fmtd(POW(-1., 2.0)));
  EXPECT_STREQ("1", fmtd(POW(-1., -2.0)));
  EXPECT_TRUE(isnan(POW(-1., NAN)));
  EXPECT_TRUE(isnan(POW(-1., -NAN)));
  EXPECT_STREQ("1", fmtd(POW(-1., INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(-1., -INFINITY)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1., __DBL_MIN__)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("1", fmtd(POW(-1., __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(1.5, 0.)));
  EXPECT_STREQ("1", fmtd(POW(1.5, -0.)));
  EXPECT_STREQ("1.22474487139159", fmtd(POW(1.5, .5)));
  EXPECT_STREQ("0.816496580927726", fmtd(POW(1.5, -.5)));
  EXPECT_STREQ("1.5", fmtd(POW(1.5, 1.)));
  EXPECT_STREQ("0.666666666666667", fmtd(POW(1.5, -1.)));
  EXPECT_STREQ("1.83711730708738", fmtd(POW(1.5, 1.5)));
  EXPECT_STREQ("0.544331053951817", fmtd(POW(1.5, -1.5)));
  EXPECT_STREQ("2.25", fmtd(POW(1.5, 2.0)));
  EXPECT_STREQ("0.444444444444444", fmtd(POW(1.5, -2.0)));
  EXPECT_TRUE(isnan(POW(1.5, NAN)));
  EXPECT_TRUE(isnan(POW(1.5, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(1.5, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(1.5, -INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(1.5, __DBL_MIN__)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(1.5, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(-1.5, 0.)));
  EXPECT_STREQ("1", fmtd(POW(-1.5, -0.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1.5, .5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1.5, -.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("-1.5", fmtd(POW(-1.5, 1.)));
  EXPECT_STREQ("-0.666666666666667", fmtd(POW(-1.5, -1.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1.5, 1.5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1.5, -1.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("2.25", fmtd(POW(-1.5, 2.0)));
  EXPECT_STREQ("0.444444444444444", fmtd(POW(-1.5, -2.0)));
  EXPECT_TRUE(isnan(POW(-1.5, NAN)));
  EXPECT_TRUE(isnan(POW(-1.5, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(-1.5, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(-1.5, -INFINITY)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-1.5, __DBL_MIN__)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(-1.5, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(+2.0, 0.)));
  EXPECT_STREQ("1", fmtd(POW(+2.0, -0.)));
  EXPECT_STREQ("1.4142135623731", fmtd(POW(+2.0, .5)));
  EXPECT_STREQ("0.707106781186548", fmtd(POW(+2.0, -.5)));
  EXPECT_STREQ("2", fmtd(POW(+2.0, 1.)));
  EXPECT_STREQ("0.5", fmtd(POW(+2.0, -1.)));
  EXPECT_STREQ("2.82842712474619", fmtd(POW(+2.0, 1.5)));
  EXPECT_STREQ("0.353553390593274", fmtd(POW(+2.0, -1.5)));
  EXPECT_STREQ("4", fmtd(POW(+2.0, 2.0)));
  EXPECT_STREQ("0.25", fmtd(POW(+2.0, -2.0)));
  EXPECT_TRUE(isnan(POW(+2.0, NAN)));
  EXPECT_TRUE(isnan(POW(+2.0, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(+2.0, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(+2.0, -INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(+2.0, __DBL_MIN__)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(+2.0, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(-2.0, 0.)));
  EXPECT_STREQ("1", fmtd(POW(-2.0, -0.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-2.0, .5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-2.0, -.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("-2", fmtd(POW(-2.0, 1.)));
  EXPECT_STREQ("-0.5", fmtd(POW(-2.0, -1.)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-2.0, 1.5)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_TRUE(isnan(POW(-2.0, -1.5)));
  EXPECT_EQ(EDOM, errno);
  EXPECT_STREQ("4", fmtd(POW(-2.0, 2.0)));
  EXPECT_STREQ("0.25", fmtd(POW(-2.0, -2.0)));
  EXPECT_TRUE(isnan(POW(-2.0, NAN)));
  EXPECT_TRUE(isnan(POW(-2.0, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(-2.0, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(-2.0, -INFINITY)));
  errno = 0;
  EXPECT_TRUE(isnan(POW(-2.0, __DBL_MIN__)));
  EXPECT_EQ(EDOM, errno);
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(-2.0, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(NAN, 0.)));
  EXPECT_STREQ("1", fmtd(POW(NAN, -0.)));
  EXPECT_TRUE(isnan(POW(NAN, .5)));
  EXPECT_TRUE(isnan(POW(NAN, -.5)));
  EXPECT_TRUE(isnan(POW(NAN, 1.)));
  EXPECT_TRUE(isnan(POW(NAN, -1.)));
  EXPECT_TRUE(isnan(POW(NAN, 1.5)));
  EXPECT_TRUE(isnan(POW(NAN, -1.5)));
  EXPECT_TRUE(isnan(POW(NAN, 2.0)));
  EXPECT_TRUE(isnan(POW(NAN, -2.0)));
  EXPECT_TRUE(isnan(POW(NAN, NAN)));
  EXPECT_TRUE(isnan(POW(NAN, -NAN)));
  EXPECT_TRUE(isnan(POW(NAN, INFINITY)));
  EXPECT_TRUE(isnan(POW(NAN, -INFINITY)));
  EXPECT_TRUE(isnan(POW(NAN, __DBL_MIN__)));
  EXPECT_TRUE(isnan(POW(NAN, __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(-NAN, 0.)));
  EXPECT_STREQ("1", fmtd(POW(-NAN, -0.)));
  EXPECT_TRUE(isnan(POW(-NAN, .5)));
  EXPECT_TRUE(isnan(POW(-NAN, -.5)));
  EXPECT_TRUE(isnan(POW(-NAN, 1.)));
  EXPECT_TRUE(isnan(POW(-NAN, -1.)));
  EXPECT_TRUE(isnan(POW(-NAN, 1.5)));
  EXPECT_TRUE(isnan(POW(-NAN, -1.5)));
  EXPECT_TRUE(isnan(POW(-NAN, 2.0)));
  EXPECT_TRUE(isnan(POW(-NAN, -2.0)));
  EXPECT_TRUE(isnan(POW(-NAN, NAN)));
  EXPECT_TRUE(isnan(POW(-NAN, -NAN)));
  EXPECT_TRUE(isnan(POW(-NAN, INFINITY)));
  EXPECT_TRUE(isnan(POW(-NAN, -INFINITY)));
  EXPECT_TRUE(isnan(POW(-NAN, __DBL_MIN__)));
  EXPECT_TRUE(isnan(POW(-NAN, __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(INFINITY, 0.)));
  EXPECT_STREQ("1", fmtd(POW(INFINITY, -0.)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, .5)));
  EXPECT_STREQ("0", fmtd(POW(INFINITY, -.5)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, 1.)));
  EXPECT_STREQ("0", fmtd(POW(INFINITY, -1.)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, 1.5)));
  EXPECT_STREQ("0", fmtd(POW(INFINITY, -1.5)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, 2.0)));
  EXPECT_STREQ("0", fmtd(POW(INFINITY, -2.0)));
  EXPECT_TRUE(isnan(POW(INFINITY, NAN)));
  EXPECT_TRUE(isnan(POW(INFINITY, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(INFINITY, -INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, __DBL_MIN__)));
  EXPECT_STREQ("inf", fmtd(POW(INFINITY, __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(-INFINITY, 0.)));
  EXPECT_STREQ("1", fmtd(POW(-INFINITY, -0.)));
  EXPECT_STREQ("inf", fmtd(POW(-INFINITY, .5)));
  EXPECT_STREQ("0", fmtd(POW(-INFINITY, -.5)));
  EXPECT_STREQ("-inf", fmtd(POW(-INFINITY, 1.)));
  EXPECT_STREQ("-0", fmtd(POW(-INFINITY, -1.)));
  EXPECT_STREQ("inf", fmtd(POW(-INFINITY, 1.5)));
  EXPECT_STREQ("0", fmtd(POW(-INFINITY, -1.5)));
  EXPECT_STREQ("inf", fmtd(POW(-INFINITY, 2.0)));
  EXPECT_STREQ("0", fmtd(POW(-INFINITY, -2.0)));
  EXPECT_TRUE(isnan(POW(-INFINITY, NAN)));
  EXPECT_TRUE(isnan(POW(-INFINITY, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(-INFINITY, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(-INFINITY, -INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(-INFINITY, __DBL_MIN__)));
  EXPECT_STREQ("inf", fmtd(POW(-INFINITY, __DBL_MAX__)));
  EXPECT_STREQ("1", fmtd(POW(__DBL_MIN__, 0.)));
  EXPECT_STREQ("1", fmtd(POW(__DBL_MIN__, -0.)));
  EXPECT_STREQ("1.49166814624004e-154", fmtd(POW(__DBL_MIN__, .5)));
  EXPECT_STREQ("6.7039039649713e+153", fmtd(POW(__DBL_MIN__, -.5)));
  EXPECT_STREQ("2.2250738585072e-308", fmtd(POW(__DBL_MIN__, 1.)));
  EXPECT_STREQ("4.49423283715579e+307", fmtd(POW(__DBL_MIN__, -1.)));
  errno = 0;
  EXPECT_STREQ("0", fmtd(POW(__DBL_MIN__, 1.5)));
  /* EXPECT_EQ(ERANGE, errno); */
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MIN__, -1.5)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("0", fmtd(POW(__DBL_MIN__, 2.0)));
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MIN__, -2.0)));
  EXPECT_TRUE(isnan(POW(__DBL_MIN__, NAN)));
  EXPECT_TRUE(isnan(POW(__DBL_MIN__, -NAN)));
  EXPECT_STREQ("0", fmtd(POW(__DBL_MIN__, INFINITY)));
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MIN__, -INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(__DBL_MIN__, __DBL_MIN__)));
  /* errno = 0; */ /* wut */
  /* EXPECT_STREQ("0", fmtd(POW(__DBL_MIN__, __DBL_MAX__))); */
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", fmtd(POW(__DBL_MAX__, 0.)));
  EXPECT_STREQ("1", fmtd(POW(__DBL_MAX__, -0.)));
  EXPECT_STREQ("1.34078079299426e+154", fmtd(POW(__DBL_MAX__, .5)));
  EXPECT_STREQ("7.45834073120021e-155", fmtd(POW(__DBL_MAX__, -.5)));
  EXPECT_STREQ("1.79769313486232e+308", fmtd(POW(__DBL_MAX__, 1.)));
  EXPECT_STREQ("5.562684646268e-309", fmtd(POW(__DBL_MAX__, -1.)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MAX__, 1.5)));
  /* EXPECT_EQ(ERANGE, errno); */
  errno = 0;
  EXPECT_STREQ("0", fmtd(POW(__DBL_MAX__, -1.5)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MAX__, 2.0)));
  errno = 0;
  EXPECT_STREQ("0", fmtd(POW(__DBL_MAX__, -2.0)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_TRUE(isnan(POW(__DBL_MAX__, NAN)));
  EXPECT_TRUE(isnan(POW(__DBL_MAX__, -NAN)));
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MAX__, INFINITY)));
  EXPECT_STREQ("0", fmtd(POW(__DBL_MAX__, -INFINITY)));
  EXPECT_STREQ("1", fmtd(POW(__DBL_MAX__, __DBL_MIN__)));
  errno = 0;
  EXPECT_STREQ("inf", fmtd(POW(__DBL_MAX__, __DBL_MAX__)));
  /* EXPECT_EQ(ERANGE, errno); */
  EXPECT_STREQ("1", gc(xasprintf("%.15g", POW(0., 0))));
  EXPECT_STREQ("1", gc(xasprintf("%.15g", POW(-0., 0))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", POW(-0., 1))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", POW(-0., 11))));
  EXPECT_STREQ("-0", gc(xasprintf("%.15g", POW(-0., 111))));
  EXPECT_STREQ("0", gc(xasprintf("%.15g", POW(-0., 2))));
  EXPECT_STREQ("0", gc(xasprintf("%.15g", POW(-0., 22))));
  EXPECT_STREQ("0", gc(xasprintf("%.15g", POW(-0., 222))));
  EXPECT_STREQ("0", gc(xasprintf("%.15g", POW(-0., 2.5))));
}
#endif

BENCH(powl, bench) {
  double _pow(double, double) asm("pow");
  float _powf(float, float) asm("powf");
  long double _powl(long double, long double) asm("powl");
  EZBENCH2("pow", donothing, _pow(.7, .2));   /* ~18ns */
  EZBENCH2("powf", donothing, _powf(.7, .2)); /* ~16ns */
  EZBENCH2("powl", donothing, _powl(.7, .2)); /* ~56ns */
}
