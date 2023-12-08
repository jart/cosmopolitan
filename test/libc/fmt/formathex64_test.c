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

char buf[100];

void SetUp(void) {
  memset(buf, 0x55, sizeof(buf));
}

TEST(FormatHex64, test1) {
  EXPECT_EQ(1, FormatHex64(buf, 0, 2) - buf);
  EXPECT_STREQ("0", buf);
}

TEST(FormatHex64, test2) {
  EXPECT_EQ(1, FormatHex64(buf, 0, 0) - buf);
  EXPECT_STREQ("0", buf);
  EXPECT_EQ(3, FormatHex64(buf, 0, 1) - buf);
  EXPECT_STREQ("0x0", buf);
}

TEST(FormatHex64, test3) {
  EXPECT_EQ(4, FormatHex64(buf, 1, 2) - buf);
  EXPECT_STREQ("0x01", buf);
}

TEST(FormatHex64, test4) {
  EXPECT_EQ(1, FormatHex64(buf, 1, 0) - buf);
  EXPECT_STREQ("1", buf);
}

TEST(FormatHex64, test5) {
  EXPECT_EQ(18, FormatHex64(buf, 01777777777777777777777UL, 2) - buf);
  EXPECT_STREQ("0xffffffffffffffff", buf);
}

TEST(FormatHex64, test6) {
  EXPECT_EQ(16, FormatHex64(buf, 01777777777777777777777UL, 0) - buf);
  EXPECT_STREQ("ffffffffffffffff", buf);
}

TEST(FormatHex64, test7) {
  EXPECT_EQ(18, FormatHex64(buf, 0xEBF2AA499B9028EAul, 2) - buf);
  EXPECT_STREQ("0xebf2aa499b9028ea", buf);
}

TEST(FormatHex64, test8) {
  EXPECT_EQ(18, FormatHex64(buf, 0x00F2AA499B9028EAul, 2) - buf);
  EXPECT_STREQ("0x00f2aa499b9028ea", buf);
}

TEST(FormatHex64, testScalesToWordSizes) {
  EXPECT_EQ(2 + 2, FormatHex64(buf, 13, 2) - buf);
  EXPECT_STREQ("0x0d", buf);
  EXPECT_EQ(4 + 2, FormatHex64(buf, 31337, 2) - buf);
  EXPECT_STREQ("0x7a69", buf);
  EXPECT_EQ(8 + 2, FormatHex64(buf, 65536, 2) - buf);
  EXPECT_STREQ("0x00010000", buf);
}

BENCH(FormatHex64, bench) {
  EZBENCH2("FormatUint64 tiny", donothing, FormatUint64(buf, 1));
  EZBENCH2("FormatOctal64 tiny", donothing, FormatOctal64(buf, 1, true));
  EZBENCH2("FormatHex64 tiny", donothing, FormatHex64(buf, 1, 2));
  EZBENCH2("FormatHex64 big", donothing,
           FormatHex64(buf, 01777777777777777777777UL, 2));
}
