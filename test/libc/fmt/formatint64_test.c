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
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"
#include "libc/limits.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(FormatInt64, test) {
  char buf[21];
  EXPECT_EQ(1, FormatInt64(buf, 0) - buf);
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(1, FormatInt64(buf, 1) - buf);
  EXPECT_STREQ("1", buf);
  EXPECT_EQ(2, FormatInt64(buf, -1) - buf);
  EXPECT_STREQ("-1", buf);
  EXPECT_EQ(19, FormatInt64(buf, INT64_MAX) - buf);
  EXPECT_STREQ("9223372036854775807", buf);
  EXPECT_EQ(20, FormatInt64(buf, INT64_MIN) - buf);
  EXPECT_STREQ("-9223372036854775808", buf);
}

TEST(FormatUint64, test) {
  char buf[21];
  EXPECT_EQ(1, FormatUint64(buf, 0) - buf);
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(4, FormatUint64(buf, 1024) - buf);
  EXPECT_STREQ("1024", buf);
  EXPECT_EQ(20, FormatUint64(buf, UINT64_MAX) - buf);
  EXPECT_STREQ("18446744073709551615", buf);
  EXPECT_EQ(19, FormatUint64(buf, INT64_MIN) - buf);
  EXPECT_STREQ("9223372036854775808", buf);
}

BENCH(itoa64radix10, bench) {
  char b[21];
  EZBENCH2("itoa64radix10", donothing, FormatUint64(b, UINT64_MAX));
}
