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

char buf[67];

void SetUp(void) {
  memset(buf, 0x55, sizeof(buf));
}

TEST(FormatBinary64, test1) {
  EXPECT_EQ(1, FormatBinary64(buf, 0, 2) - buf);
  EXPECT_STREQ("0", buf);
}

TEST(FormatBinary64, test2) {
  EXPECT_EQ(1, FormatBinary64(buf, 0, 0) - buf);
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(3, FormatBinary64(buf, 0, 1) - buf);
  EXPECT_STREQ("0b0", buf);
}

TEST(FormatBinary64, test3) {
  EXPECT_EQ(3, FormatBinary64(buf, 1, 2) - buf);
  EXPECT_STREQ("0b1", buf);
}

TEST(FormatBinary64, test4) {
  EXPECT_EQ(1, FormatBinary64(buf, 1, 0) - buf);
  EXPECT_STREQ("1", buf);
}

TEST(FormatBinary64, test5) {
  EXPECT_EQ(66, FormatBinary64(buf, 01777777777777777777777UL, 2) - buf);
  EXPECT_STREQ(
      "0b1111111111111111111111111111111111111111111111111111111111111111",
      buf);
}

TEST(FormatBinary64, test6) {
  EXPECT_EQ(64, FormatBinary64(buf, 01777777777777777777777UL, 0) - buf);
  EXPECT_STREQ(
      "1111111111111111111111111111111111111111111111111111111111111111", buf);
}

TEST(FormatBinary64, test7) {
  EXPECT_EQ(66, FormatBinary64(buf, 0xEBF2AA499B9028EAul, 2) - buf);
  EXPECT_STREQ(
      "0b1110101111110010101010100100100110011011100100000010100011101010",
      buf);
}

TEST(FormatBinary64, test8) {
  EXPECT_EQ(66, FormatBinary64(buf, 0x00F2AA499B9028EAul, 2) - buf);
  EXPECT_STREQ(
      "0b0000000011110010101010100100100110011011100100000010100011101010",
      buf);
}

TEST(FormatBinary64, testScalesToWordSizes) {
  EXPECT_EQ(8 + 2, FormatBinary64(buf, 13, 2) - buf);
  EXPECT_STREQ("0b00001101", buf);
  EXPECT_EQ(16 + 2, FormatBinary64(buf, 31337, 2) - buf);
  EXPECT_STREQ("0b0111101001101001", buf);
  EXPECT_EQ(32 + 2, FormatBinary64(buf, 65536, 2) - buf);
  EXPECT_STREQ("0b00000000000000010000000000000000", buf);
}

BENCH(FormatBinary64, bench) {
  EZBENCH2("FormatUint64 tiny", donothing, FormatUint64(buf, 1));
  EZBENCH2("FormatOctal64 tiny", donothing, FormatOctal64(buf, 1, true));
  EZBENCH2("FormatBinary64 tiny", donothing, FormatBinary64(buf, 1, 2));
  EZBENCH2("FormatUint64 big", donothing,
           FormatUint64(buf, 01777777777777777777777UL));
  EZBENCH2("FormatOctal64 big", donothing,
           FormatOctal64(buf, 01777777777777777777777UL, true));
  EZBENCH2("FormatBinary64 big", donothing,
           FormatBinary64(buf, 01777777777777777777777UL, 2));
}
