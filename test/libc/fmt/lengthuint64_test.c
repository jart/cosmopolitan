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
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(LengthUint64Thousands, test) {
  int i;
  char s[27];
  static const uint64_t v[] = {
      9,
      10,
      11,
      99,
      100,
      101,
      999,
      1000,
      1001,
      999999,
      1000000,
      1000001,
      UINT64_MIN,
      UINT64_MAX,
      UINT64_MIN + 1,
      UINT64_MAX - 1,
      0x8000000000000001ull,
      0x8000000000000000ull,
  };
  for (i = 0; i < ARRAYLEN(v); ++i) {
    FormatUint64Thousands(s, v[i]);
    ASSERT_EQ(strlen(s), LengthUint64Thousands(v[i]), "%,lu", v[i]);
  }
}

TEST(LengthInt64Thousands, test) {
  int i;
  char s[27];
  static const int64_t v[] = {
      -1000001,
      -1000000,
      -999999,
      -1001,
      -1000,
      -999,
      -101,
      -100,
      -99,
      -11,
      -10,
      -9,
      -1,
      0,
      1,
      9,
      10,
      11,
      99,
      100,
      101,
      999,
      1000,
      1001,
      999999,
      1000000,
      1000001,
      INT64_MIN,
      INT64_MAX,
      INT64_MIN + 1,
      INT64_MAX - 1,
  };
  for (i = 0; i < ARRAYLEN(v); ++i) {
    FormatInt64Thousands(s, v[i]);
    ASSERT_EQ(strlen(s), LengthInt64Thousands(v[i]), "%,ld\n\t%s\n\t%lx", v[i],
              s, v[i]);
  }
}

BENCH(LengthInt64, bench) {
  unsigned LengthInt64_(int64_t) asm("__LengthInt64");
  unsigned LengthUint64_(uint64_t) asm("__LengthUint64");
  unsigned LengthInt64Thousands_(int64_t) asm("__LengthInt64Thousands");
  unsigned LengthUint64Thousands_(uint64_t) asm("__LengthUint64Thousands");
  EZBENCH2("LengthInt64", donothing, LengthInt64_(INT64_MIN));
  EZBENCH2("LengthUint64", donothing, LengthUint64_(UINT64_MAX));
  EZBENCH2("LengthInt64Thousands", donothing, LengthInt64Thousands_(INT64_MIN));
  EZBENCH2("LengthUint64Thousands", donothing,
           LengthUint64Thousands_(UINT64_MAX));
}
