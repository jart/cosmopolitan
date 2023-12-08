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
#include "net/http/ip.h"

TEST(ParseIp, test) {
  EXPECT_EQ(-1, ParseIp("", -1));
  EXPECT_EQ(0x00000000, ParseIp("0.0.0.0", -1));
  EXPECT_EQ(0x01020304, ParseIp("1.2.3.4", -1));
  EXPECT_EQ(0x01020304, ParseIp("16909060", -1));
  EXPECT_EQ(0x80020304, ParseIp("128.2.3.4", -1));
  EXPECT_EQ(0xFFFFFFFF, ParseIp("255.255.255.255", -1));
  EXPECT_EQ(0xcb007100, ParseIp("203.0.113.0", -1));
  EXPECT_EQ(0x00000000, ParseIp("...", -1)); /* meh */
  EXPECT_EQ(0x80000304, ParseIp("128.0.3.4", -1));
  EXPECT_EQ(0x80000304, ParseIp("128..3.4", -1));
  EXPECT_EQ(-1, ParseIp("4294967296", -1));
  EXPECT_EQ(-1, ParseIp("255.255.255.256", -1));
  EXPECT_EQ(-1, ParseIp("256.255.255.255", -1));
  EXPECT_EQ(-1, ParseIp("hello", -1));
  EXPECT_EQ(-1, ParseIp("hello\177", -1));
  EXPECT_EQ(-1, ParseIp("hello.example\300\200", -1));
  EXPECT_EQ(-1, ParseIp(".e", -1));
  EXPECT_EQ(-1, ParseIp("e.", -1));
  EXPECT_EQ(-1, ParseIp(".hi.example", -1));
  EXPECT_EQ(-1, ParseIp("hi..example", -1));
  EXPECT_EQ(-1, ParseIp("hi-there.example", -1));
  EXPECT_EQ(-1, ParseIp("_there.example", -1));
  EXPECT_EQ(-1, ParseIp("-there.example", -1));
  EXPECT_EQ(-1, ParseIp("there-.example", -1));
  EXPECT_EQ(-1, ParseIp("ther#e.example", -1));
  EXPECT_EQ(-1, ParseIp("localhost", -1));
  EXPECT_EQ(-1, ParseIp("hello.example", -1));
  EXPECT_EQ(-1, ParseIp("hello..example", -1));
}
