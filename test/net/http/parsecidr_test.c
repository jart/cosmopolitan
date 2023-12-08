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
#include "libc/testlib/testlib.h"
#include "net/http/ip.h"

TEST(ParseCidr, test) {
  struct Cidr in;
  in = ParseCidr("10.10.10.1/24", -1);
  EXPECT_EQ(0x0a0a0a01, in.addr);
  EXPECT_EQ(24, in.cidr);
  in = ParseCidr("168430081/1", -1);
  EXPECT_EQ(0x0a0a0a01, in.addr);
  EXPECT_EQ(1, in.cidr);
}

TEST(ParseCidr, noCidr_defaultsTo32) {
  struct Cidr in;
  in = ParseCidr("10.10.10.255", -1);
  EXPECT_EQ(0x0a0a0aff, in.addr);
  EXPECT_EQ(32, in.cidr);
}

TEST(ParseCidr, badIp_returnsNeg1) {
  struct Cidr in;
  in = ParseCidr("10.10.10.a", -1);
  EXPECT_EQ(-1, in.addr);
  in = ParseCidr("10.10.10.256", -1);
  EXPECT_EQ(-1, in.addr);
  in = ParseCidr("10.10.10.256/24", -1);
  EXPECT_EQ(-1, in.addr);
}

TEST(ParseCidr, badCidr_returnsNeg1) {
  struct Cidr in;
  in = ParseCidr("10.10.10.1/", -1);
  EXPECT_EQ(-1, in.addr);
  in = ParseCidr("10.10.10.1/a", -1);
  EXPECT_EQ(-1, in.addr);
  in = ParseCidr("10.10.10.1/0", -1);
  EXPECT_EQ(-1, in.addr);
  in = ParseCidr("10.10.10.1/33", -1);
  EXPECT_EQ(-1, in.addr);
}
