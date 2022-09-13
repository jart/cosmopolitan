/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/mem/gc.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define hypotl(x, y) \
  hypotl(VEIL("t", (long double)(x)), VEIL("t", (long double)(y)))
#define hypot(x, y)  hypot(VEIL("x", (double)(x)), VEIL("x", (double)(y)))
#define hypotf(x, y) hypotf(VEIL("x", (float)(x)), VEIL("x", (float)(y)))

TEST(hypot, test) {
  EXPECT_STREQ("0", gc(xdtoa(hypot(0, 0))));
  EXPECT_STREQ("3", gc(xdtoa(hypot(3, 0))));
  EXPECT_STREQ("3", gc(xdtoa(hypot(0, 3))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(-4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(-3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(4, -3))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(-3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(hypot(-4, -3))));
  EXPECT_STREQ("1.4142135623731", gc(xdtoa(hypot(1, 1))));
  EXPECT_STREQ("1.4142135623731", gc(xdtoa(hypot(1, -1))));
  EXPECT_STREQ("1.4142135623731", gc(xdtoa(hypot(-1, 1))));
  EXPECT_STREQ("1.41421362601271", gc(xdtoa(hypot(1.0000001, .99999999))));
  EXPECT_STREQ("1.41421362601271", gc(xdtoa(hypot(.99999999, 1.0000001))));
  EXPECT_STREQ("1.4142135623731e+154", gc(xdtoa(hypot(1e154, 1e154))));
  EXPECT_STREQ("NAN", gc(xdtoa(hypot(0, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoa(hypot(NAN, 0))));
  EXPECT_STREQ("NAN", gc(xdtoa(hypot(NAN, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(hypot(INFINITY, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(hypot(0, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(hypot(INFINITY, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(hypot(NAN, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoa(hypot(INFINITY, INFINITY))));
}

TEST(hypotf, test) {
  EXPECT_STREQ("0", gc(xdtoa(hypotf(0, 0))));
  EXPECT_STREQ("3", gc(xdtoa(hypotf(3, 0))));
  EXPECT_STREQ("3", gc(xdtoa(hypotf(0, 3))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(-4, 3))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(-3, 4))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(4, -3))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(-3, -4))));
  EXPECT_STREQ("5", gc(xdtoa(hypotf(-4, -3))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(hypotf(1, 1))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(hypotf(1, -1))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(hypotf(-1, 1))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(hypotf(1.000001, 0.999999))));
  EXPECT_STREQ("1.41421", gc(xdtoaf(hypotf(0.999999, 1.000001))));
  EXPECT_STREQ("1.41421e+38", gc(xdtoaf(hypotf(1e38, 1e38))));
  EXPECT_STREQ("NAN", gc(xdtoaf(hypotf(0, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoaf(hypotf(NAN, 0))));
  EXPECT_STREQ("NAN", gc(xdtoaf(hypotf(NAN, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(hypotf(INFINITY, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(hypotf(0, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(hypotf(INFINITY, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(hypotf(NAN, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoaf(hypotf(INFINITY, INFINITY))));
}

TEST(hypotll, test) {
  EXPECT_STREQ("0", gc(xdtoal(hypotl(0, 0))));
  EXPECT_STREQ("3", gc(xdtoal(hypotl(3, 0))));
  EXPECT_STREQ("3", gc(xdtoal(hypotl(0, 3))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(3, 4))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(4, 3))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(3, -4))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(-4, 3))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(-3, 4))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(4, -3))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(-3, -4))));
  EXPECT_STREQ("5", gc(xdtoal(hypotl(-4, -3))));
  EXPECT_STREQ("1.414213562373095", gc(xdtoal(hypotl(1, 1))));
  EXPECT_STREQ("1.414213562373095", gc(xdtoal(hypotl(1, -1))));
  EXPECT_STREQ("1.414213562373095", gc(xdtoal(hypotl(-1, 1))));
  EXPECT_STREQ("1.414213626012708", gc(xdtoal(hypotl(1.0000001, .99999999))));
  EXPECT_STREQ("1.414213626012708", gc(xdtoal(hypotl(.99999999, 1.0000001))));
  EXPECT_STREQ("1.414213562373095e+4931", gc(xdtoal(hypotl(1e4931L, 1e4931L))));
  EXPECT_STREQ("NAN", gc(xdtoal(hypotl(0, NAN))));
  EXPECT_STREQ("NAN", gc(xdtoal(hypotl(NAN, 0))));
  EXPECT_STREQ("NAN", gc(xdtoal(hypotl(NAN, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(hypotl(INFINITY, 0))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(hypotl(0, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(hypotl(INFINITY, NAN))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(hypotl(NAN, INFINITY))));
  EXPECT_STREQ("INFINITY", gc(xdtoal(hypotl(INFINITY, INFINITY))));
}

/*
hypot (musl)               l:        53𝑐        17𝑛𝑠   m:        85𝑐        27𝑛𝑠
hypot                      l:        39𝑐        13𝑛𝑠   m:        66𝑐        21𝑛𝑠
hypotf                     l:        25𝑐         8𝑛𝑠   m:        55𝑐        18𝑛𝑠
hypotl                     l:        43𝑐        14𝑛𝑠   m:        74𝑐        24𝑛𝑠
*/

BENCH(hypot, bench) {
  volatile double a = 2;
  volatile double b = 3;
  EZBENCH2("hypotf", donothing, EXPROPRIATE(hypotf(a, b)));
  EZBENCH2("hypot", donothing, EXPROPRIATE(hypot(a, b)));
  EZBENCH2("hypotl", donothing, EXPROPRIATE(hypotl(a, b)));
}
