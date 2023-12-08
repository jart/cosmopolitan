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
#include "libc/testlib/testlib.h"

char buf[13];

void SetUp(void) {
  memset(buf, 0x55, sizeof(buf));
}

TEST(FormatOctal32, test1) {
  EXPECT_EQ(1, FormatOctal32(buf, 0, true) - buf);
  EXPECT_STREQ("0", buf);
}

TEST(FormatOctal32, test2) {
  EXPECT_EQ(1, FormatOctal32(buf, 0, false) - buf);
  EXPECT_STREQ("0", buf);
}

TEST(FormatOctal32, test3) {
  EXPECT_EQ(2, FormatOctal32(buf, 1, true) - buf);
  EXPECT_STREQ("01", buf);
}

TEST(FormatOctal32, test4) {
  EXPECT_EQ(1, FormatOctal32(buf, 1, false) - buf);
  EXPECT_STREQ("1", buf);
}

TEST(FormatOctal32, test5) {
  EXPECT_EQ(12, FormatOctal32(buf, 037777777777, true) - buf);
  EXPECT_STREQ("037777777777", buf);
}

TEST(FormatOctal32, test6) {
  EXPECT_EQ(11, FormatOctal32(buf, 037777777777, false) - buf);
  EXPECT_STREQ("37777777777", buf);
}
