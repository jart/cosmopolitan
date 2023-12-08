/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

char buf[24];

void SetUp(void) {
  memset(buf, 0x55, sizeof(buf));
}

TEST(FormatOctal64, test1) {
  EXPECT_EQ(1, FormatOctal64(buf, 0, true) - buf);
  EXPECT_STREQ("0", buf);
}

TEST(FormatOctal64, test2) {
  EXPECT_EQ(1, FormatOctal64(buf, 0, false) - buf);
  EXPECT_STREQ("0", buf);
}

TEST(FormatOctal64, test3) {
  EXPECT_EQ(2, FormatOctal64(buf, 1, true) - buf);
  EXPECT_STREQ("01", buf);
}

TEST(FormatOctal64, test4) {
  EXPECT_EQ(1, FormatOctal64(buf, 1, false) - buf);
  EXPECT_STREQ("1", buf);
}

TEST(FormatOctal64, test5) {
  EXPECT_EQ(23, FormatOctal64(buf, 01777777777777777777777UL, true) - buf);
  EXPECT_STREQ("01777777777777777777777", buf);
}

TEST(FormatOctal64, test6) {
  EXPECT_EQ(22, FormatOctal64(buf, 01777777777777777777777UL, false) - buf);
  EXPECT_STREQ("1777777777777777777777", buf);
}

BENCH(FormatOctal64, bench) {
  EZBENCH2("FormatUint64", donothing,
           FormatUint64(buf, 01777777777777777777777UL));
  EZBENCH2("FormatOctal64", donothing,
           FormatOctal64(buf, 01777777777777777777777UL, true));
  EZBENCH2("FormatOctal32", donothing, FormatOctal32(buf, 037777777777U, true));
}
