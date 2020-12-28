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
#include "libc/bits/bits.h"
#include "libc/bits/progn.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

#define L(x) ((int64_t)(x))
#define S(x) ((int128_t)(x))
#define U(x) ((uint128_t)(x))

TEST(division, testUnsigned) {
  volatile uint128_t x;
  EXPECT_EQ(U(20769187431582143),
            PROGN(x = U(1000000000123123123), (U(1125899906842624) << 64) / x));
  EXPECT_EQ((U(26807140639110) << 64) | U(1756832768924719201),
            PROGN(x = U(42), (U(1125899906842624) << 64) / x));
}

TEST(division, testSigned) {
  volatile int128_t x;
  EXPECT_EQ(S(20769187431582143),
            PROGN(x = S(1000000000123123123), (S(1125899906842624) << 64) / x));
  EXPECT_EQ(S(26807140639110) << 64 | S(1756832768924719201),
            PROGN(x = S(42), (S(1125899906842624) << 64) / x));
}

BENCH(divmodti4, bench) {
  volatile int128_t x;
  EZBENCH2("divmodti4 small / small", donothing,
           PROGN(x = S(42), x = S(112589990684) / x));
  EZBENCH2("divmodti4 small / large", donothing,
           PROGN(x = U(123) << 64 | 123, x = S(112589990684) / x));
  EZBENCH2("divmodti4 large / small", donothing,
           PROGN(x = 123, x = (S(1125899906842624) << 64 | 334) / x));
  EZBENCH2(
      "divmodti4 large / large", donothing,
      PROGN(x = U(123) << 64 | 123, x = (S(1125899906842624) << 64 | 334) / x));
}

BENCH(idiv32, bench) {
  volatile int32_t x;
  EZBENCH2("idiv32", donothing,
           PROGN(x = L(1000000000123123123), x = L(1125899906842624) / x));
}

BENCH(idiv64, bench) {
  volatile int64_t x;
  EZBENCH2("idiv64", donothing,
           PROGN(x = L(1000000000123123123), x = L(1125899906842624) / x));
}
