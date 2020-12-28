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
#include "libc/runtime/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/tinymath/tinymath.h"
#include "libc/x/x.h"

TEST(powl, testLongDouble) {
  /* .4248496805467504836322459796959084815827285786480897 */
  EXPECT_STARTSWITH(".4248496805467504", gc(xdtoa(powl(0.7, 2.4))));
  EXPECT_STARTSWITH(".4248496805467504", gc(xdtoa(tinymath_powl(0.7, 2.4))));
}

TEST(powl, testDouble) {
  EXPECT_STARTSWITH(".4248496805467504", gc(xdtoa(pow(0.7, 2.4))));
  EXPECT_STARTSWITH(".4248496805467504", gc(xdtoa(tinymath_pow(0.7, 2.4))));
}

TEST(powl, testFloat) {
  EXPECT_STARTSWITH(".4248496", gc(xdtoa(powf(0.7f, 2.4f))));
  EXPECT_STARTSWITH(".4248496", gc(xdtoa(tinymath_powf(0.7f, 2.4f))));
}

static long double do_powl(void) {
  return CONCEAL("t", tinymath_powl(CONCEAL("t", 0.7), CONCEAL("t", 0.2)));
}

static double do_pow(void) {
  return CONCEAL("x", tinymath_pow(CONCEAL("x", 0.7), CONCEAL("x", 0.2)));
}

static float do_powf(void) {
  return CONCEAL("x", tinymath_powf(CONCEAL("x", 0.7f), CONCEAL("x", 0.2f)));
}

BENCH(powl, bench) {
  EZBENCH2("powl", donothing, do_powl()); /* ~61ns */
  EZBENCH2("pow", donothing, do_pow());   /* ~64ns */
  EZBENCH2("powf", donothing, do_powf()); /* ~64ns */
}
