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
#include "libc/math.h"
#include "libc/runtime/gc.h"
#include "libc/runtime/pc.internal.h"
#include "libc/sysv/consts/sa.h"
#include "libc/sysv/consts/sig.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double powa(double x, double y) {
  return exp2(fmod(y * log2(x), 1)) * exp2(y);
}

TEST(powl, testLongDouble) {
  EXPECT_TRUE(isnan(powl(-1, 1.1)));
  EXPECT_STREQ("1e+4932", gc(xdtoal(powl(10, 4932))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(powl(10, 4933))));
  EXPECT_STREQ("0", gc(xdtoal(powl(10, -5000))));
  EXPECT_STREQ("1.063382396627933e+37", gc(xdtoal(powl(2, 123))));
  /* .4248496805467504836322459796959084815827285786480897 */
  EXPECT_STARTSWITH(".4248496805467504", gc(xdtoal(powl(0.7, 2.4))));
}

TEST(powl, testDouble) {
  EXPECT_STREQ("1e+308", gc(xdtoa(pow(10, 308))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(pow(10, 309))));
  EXPECT_STARTSWITH(".42484968054675", gc(xdtoa(pow(0.7, 2.4))));
}

TEST(powl, testFloat) {
  EXPECT_STARTSWITH(".4248496", gc(xdtoa(powf(0.7f, 2.4f))));
}

BENCH(powl, bench) {
  double _pow(double, double) asm("pow");
  float _powf(float, float) asm("powf");
  long double _powl(long double, long double) asm("powl");
  EZBENCH2("pow", donothing, _pow(.7, .2));   /* ~51ns */
  EZBENCH2("powf", donothing, _powf(.7, .2)); /* ~52ns */
  EZBENCH2("powl", donothing, _powl(.7, .2)); /* ~53ns */
}
