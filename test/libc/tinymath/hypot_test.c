/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

double _hypot(double, double) asm("hypot");
float _hypotf(float, float) asm("hypotf");
long double _hypotl(long double, long double) asm("hypotl");

TEST(hypot, test) {
  EXPECT_STREQ("0", gc(xdtoa(_hypot(0, 0))));
  EXPECT_STREQ("3", gc(xdtoa(_hypot(3, 0))));
  EXPECT_STREQ("3", gc(xdtoa(_hypot(0, 3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(-4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(-3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(4, -3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(-3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypot(-4, -3))));
  EXPECT_STREQ("1.4142135623731", gc(xdtoa(_hypot(1, 1))));
  EXPECT_STREQ("1.4142135623731", gc(xdtoa(_hypot(1, -1))));
  EXPECT_STREQ("1.4142135623731", gc(xdtoa(_hypot(-1, 1))));
  EXPECT_STREQ("1.41421362601271", gc(xdtoa(_hypot(1.0000001, .99999999))));
  EXPECT_STREQ("1.41421362601271", gc(xdtoa(_hypot(.99999999, 1.0000001))));
  EXPECT_STREQ("1.4142135623731e+154", gc(xdtoa(_hypot(1e154, 1e154))));
  EXPECT_STREQ("NAN", gc(xdtoa(_hypot(0, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoa(_hypot(NAN, 0))));
  EXPECT_STREQ("NAN", gc(xdtoa(_hypot(NAN, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_hypot(INFINITY, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_hypot(0, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_hypot(INFINITY, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_hypot(NAN, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(_hypot(INFINITY, INFINITY))));
}

TEST(hypotf, test) {
  EXPECT_STREQ("0", gc(xdtoa(_hypotf(0, 0))));
  EXPECT_STREQ("3", gc(xdtoa(_hypotf(3, 0))));
  EXPECT_STREQ("3", gc(xdtoa(_hypotf(0, 3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(-4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(-3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(4, -3))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(-3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(_hypotf(-4, -3))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(_hypotf(1, 1))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(_hypotf(1, -1))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(_hypotf(-1, 1))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(_hypotf(1.000001, 0.999999))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(_hypotf(0.999999, 1.000001))));
  EXPECT_STREQ("1.41421e+38", gc(xdtoaf(_hypotf(1e38, 1e38))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_hypotf(0, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_hypotf(NAN, 0))));
  EXPECT_STREQ("NAN", gc(xdtoaf(_hypotf(NAN, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_hypotf(INFINITY, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_hypotf(0, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_hypotf(INFINITY, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_hypotf(NAN, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(_hypotf(INFINITY, INFINITY))));
}

TEST(hypotll, test) {
  EXPECT_STREQ("0", gc(xdtoal(_hypotl(0, 0))));
  EXPECT_STREQ("3", gc(xdtoal(_hypotl(3, 0))));
  EXPECT_STREQ("3", gc(xdtoal(_hypotl(0, 3))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(3, 4))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(4, 3))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(3, -4))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(-4, 3))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(-3, 4))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(4, -3))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(-3, -4))));
  EXPECT_STREQ("5", gc(xdtoal(_hypotl(-4, -3))));
  EXPECT_STREQ("1.414213562373095", gc(xdtoal(_hypotl(1, 1))));
  EXPECT_STREQ("1.414213562373095", gc(xdtoal(_hypotl(1, -1))));
  EXPECT_STREQ("1.414213562373095", gc(xdtoal(_hypotl(-1, 1))));
  EXPECT_STREQ("1.414213626012708", gc(xdtoal(_hypotl(1.0000001, .99999999))));
  EXPECT_STREQ("1.414213626012708", gc(xdtoal(_hypotl(.99999999, 1.0000001))));
  EXPECT_STREQ("1.414213562373095e+4931",
               gc(xdtoal(_hypotl(1e4931L, 1e4931L))));
  EXPECT_STREQ("NAN", gc(xdtoal(_hypotl(0, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoal(_hypotl(NAN, 0))));
  EXPECT_STREQ("NAN", gc(xdtoal(_hypotl(NAN, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_hypotl(INFINITY, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_hypotl(0, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_hypotl(INFINITY, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_hypotl(NAN, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(_hypotl(INFINITY, INFINITY))));
}

/*
_hypot (musl)               l:        53𝑐        17𝑛𝑠   m:        85𝑐 27𝑛𝑠
_hypot                      l:        39𝑐        13𝑛𝑠   m:        66𝑐 21𝑛𝑠
_hypotf                     l:        25𝑐         8𝑛𝑠   m:        55𝑐 18𝑛𝑠
_hypotl                     l:        43𝑐        14𝑛𝑠   m:        74𝑐 24𝑛𝑠
*/

BENCH(_hypot, bench) {
  volatile double a = 2;
  volatile double b = 3;
  EZBENCH2("hypotf", donothing, __expropriate(_hypotf(a, b)));
  EZBENCH2("hypot", donothing, __expropriate(_hypot(a, b)));
  EZBENCH2("hypotl", donothing, __expropriate(_hypotl(a, b)));
}
