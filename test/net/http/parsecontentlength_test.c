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
#include "libc/testlib/testlib.h"
#include "net/http/http.h"

TEST(ParseContentLength, test) {
  EXPECT_EQ(-1, ParseContentLength(0, 0));
  EXPECT_EQ(-1, ParseContentLength("", 0));
  EXPECT_EQ(-1, ParseContentLength("-1", 2));
  EXPECT_EQ(-1, ParseContentLength("-2", 2));
  EXPECT_EQ(-1, ParseContentLength("e", -1));
  EXPECT_EQ(-1, ParseContentLength(",", -1));
  EXPECT_EQ(-1, ParseContentLength("\0", 1));
  EXPECT_EQ(0, ParseContentLength("0", 1));
  EXPECT_EQ(1, ParseContentLength("1", 1));
  EXPECT_EQ(42, ParseContentLength("42, 42", -1)); /* RFC7230 § 3.3.2 */
  EXPECT_EQ(0x000000ffffffffff, ParseContentLength("1099511627775", -1));
  EXPECT_EQ(-1, ParseContentLength("1099511627776", -1));
}
