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
#include "libc/calls/struct/sigaction.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/ucontext.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/pc.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

int rando;
void SetUp(void) {
  rando = rand() & 0xffff;
}

TEST(powl, test) {
  EXPECT_STREQ("27", gc(xdtoal(powl(3, 3))));
  EXPECT_STREQ("-27", gc(xdtoal(powl(-3, 3))));
  EXPECT_STREQ("1e+4932", gc(xdtoal(powl(10, 4932))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(10, 4933))));
  EXPECT_STREQ("0", gc(xdtoal(powl(10, -5000))));
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
}

TEST(pow, test) {
  EXPECT_STREQ("27", gc(xdtoa(pow(3, 3))));
  EXPECT_STREQ("-27", gc(xdtoa(pow(-3, 3))));
  EXPECT_STREQ("1e+308", gc(xdtoa(pow(10, 308))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(10, 309))));
  EXPECT_STREQ("0", gc(xdtoa(pow(10, -5000))));
  EXPECT_STREQ("1.06338239662793e+37", gc(xdtoa(pow(2, 123))));
  EXPECT_STARTSWITH(".42484968054675", gc(xdtoa(pow(.7, 2.4))));
  EXPECT_STREQ("1", gc(xdtoa(pow(1, NAN))));
  EXPECT_STREQ("1", gc(xdtoa(pow(1, rando))));
  EXPECT_STREQ("1", gc(xdtoa(pow(NAN, 0))));
  EXPECT_STREQ("1", gc(xdtoa(pow(rando, 0))));
  EXPECT_STREQ("0", gc(xdtoa(pow(0, 1))));
  EXPECT_STREQ("0", gc(xdtoa(pow(0, 2))));
  EXPECT_STREQ("0", gc(xdtoa(pow(0, 2.1))));
  EXPECT_STREQ("1", gc(xdtoa(pow(-1, INFINITY))));
  EXPECT_STREQ("1", gc(xdtoa(pow(-1, -INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(1. / MAX(2, rando), -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoa(pow(1.1, -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoa(pow(MAX(2, rando), -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoa(pow(1. / MAX(2, rando), INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(MAX(2, rando), INFINITY))));
  EXPECT_STREQ("-0", gc(xdtoa(pow(-INFINITY, -1))));
  EXPECT_STREQ("0", gc(xdtoa(pow(-INFINITY, -1.1))));
  EXPECT_STREQ("0", gc(xdtoa(pow(-INFINITY, -2))));
  EXPECT_STREQ("0", gc(xdtoa(pow(-INFINITY, -2.1))));
  EXPECT_STREQ("-0", gc(xdtoa(pow(-INFINITY, -3))));
  EXPECT_STREQ("0", gc(xdtoa(pow(-INFINITY, -3.1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(pow(-INFINITY, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(-INFINITY, 1.1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(-INFINITY, 2))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(-INFINITY, 2.1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(pow(-INFINITY, 3))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(-INFINITY, 3.1))));
  EXPECT_STREQ("0", gc(xdtoa(pow(INFINITY, -1))));
  EXPECT_STREQ("0", gc(xdtoa(pow(INFINITY, -.1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(INFINITY, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(INFINITY, .1))));
  EXPECT_STREQ("1", gc(xdtoa(pow(INFINITY, 0))));
  EXPECT_STREQ("1", gc(xdtoa(pow(INFINITY, -0.))));
  EXPECT_STREQ("1", gc(xdtoa(pow(0, 0))));
  EXPECT_STREQ("1", gc(xdtoa(pow(0, -0.))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(0, -(MAX(rando, 1) | 1)))));
  EXPECT_STREQ("-INFINITY", gc(xdtoa(pow(-0., -(MAX(rando, 1) | 1)))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(0, -(rando & -2)))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(-0., -(rando & -2)))));
  EXPECT_STREQ("-.333333333333333", gc(xdtoa(pow(-3, -1))));
  EXPECT_STREQ(".111111111111111", gc(xdtoa(pow(-3, -2))));
}

TEST(powf, test) {
  EXPECT_STREQ("27", gc(xdtoaf(powf(3, 3))));
  EXPECT_STREQ("-27", gc(xdtoaf(powf(-3, 3))));
  EXPECT_STREQ("1e+38", gc(xdtoaf(powf(10, 38))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(10, 39))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(10, -5000))));
  EXPECT_STREQ("1.06338e+37", gc(xdtoaf(powf(2, 123))));
  EXPECT_STARTSWITH(".42485", gc(xdtoaf(powf(.7, 2.4))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(1, NAN))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(1, rando))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(NAN, 0))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(rando, 0))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(0, 1))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(0, 2))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(0, 2.1))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(-1, INFINITY))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(-1, -INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(1. / MAX(2, rando), -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(1.1, -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(MAX(2, rando), -INFINITY))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(1. / MAX(2, rando), INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(MAX(2, rando), INFINITY))));
  EXPECT_STREQ("-0", gc(xdtoaf(powf(-INFINITY, -1))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(-INFINITY, -1.1))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(-INFINITY, -2))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(-INFINITY, -2.1))));
  EXPECT_STREQ("-0", gc(xdtoaf(powf(-INFINITY, -3))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(-INFINITY, -3.1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(powf(-INFINITY, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(-INFINITY, 1.1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(-INFINITY, 2))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(-INFINITY, 2.1))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(powf(-INFINITY, 3))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(-INFINITY, 3.1))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(INFINITY, -1))));
  EXPECT_STREQ("0", gc(xdtoaf(powf(INFINITY, -.1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(INFINITY, 1))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(INFINITY, .1))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(INFINITY, 0))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(INFINITY, -0.))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(0, 0))));
  EXPECT_STREQ("1", gc(xdtoaf(powf(0, -0.))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(0, -(MAX(rando, 1) | 1)))));
  EXPECT_STREQ("-INFINITY", gc(xdtoaf(powf(-0., -(MAX(rando, 1) | 1)))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(0, -(rando & -2)))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(powf(-0., -(rando & -2)))));
  EXPECT_STREQ("-.333333", gc(xdtoaf(powf(-3, -1))));
  EXPECT_STREQ(".111111", gc(xdtoaf(powf(-3, -2))));
}

BENCH(powl, bench) {
  double _pow(double, double) asm("pow");
  float _powf(float, float) asm("powf");
  long double _powl(long double, long double) asm("powl");
  EZBENCH2("pow", donothing, _pow(.7, .2));   /* ~51ns */
  EZBENCH2("powf", donothing, _powf(.7, .2)); /* ~52ns */
  EZBENCH2("powl", donothing, _powl(.7, .2)); /* ~53ns */
}
