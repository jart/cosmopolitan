/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2026 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "net/http/ip.h"

static const uint8_t kDoc[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                 0,    0,    0,    0,    0, 0, 0, 0};
static const uint8_t kDoc1[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                  0,    0,    0,    0,    0, 0, 0, 1};

TEST(ParseCidr6, test) {
  struct Cidr6 c = ParseCidr6("2001:db8::/32", -1);
  EXPECT_EQ(32, c.cidr);
  EXPECT_EQ(0, memcmp(c.addr, kDoc, 16));
}

TEST(ParseCidr6, noCidr_defaultsTo128) {
  struct Cidr6 c = ParseCidr6("2001:db8::1", -1);
  EXPECT_EQ(128, c.cidr);
  EXPECT_EQ(0, memcmp(c.addr, kDoc1, 16));
}

TEST(ParseCidr6, hostBitsAreNotMasked) {
  struct Cidr6 c = ParseCidr6("2001:db8::1/64", -1);
  EXPECT_EQ(64, c.cidr);
  EXPECT_EQ(0, memcmp(c.addr, kDoc1, 16));
}

TEST(ParseCidr6, badIp) {
  EXPECT_EQ(-1, ParseCidr6("2001:db8::g/32", -1).cidr);
  EXPECT_EQ(-1, ParseCidr6("1.2.3.4/24", -1).cidr);
  EXPECT_EQ(-1, ParseCidr6("", -1).cidr);
}

TEST(ParseCidr6, badCidr) {
  EXPECT_EQ(-1, ParseCidr6("2001:db8::/", -1).cidr);
  EXPECT_EQ(-1, ParseCidr6("2001:db8::/a", -1).cidr);
  EXPECT_EQ(-1, ParseCidr6("2001:db8::/0", -1).cidr);
  EXPECT_EQ(-1, ParseCidr6("2001:db8::/129", -1).cidr);
  EXPECT_EQ(-1, ParseCidr6("2001:db8::/32x", -1).cidr);
  EXPECT_EQ(128, ParseCidr6("2001:db8::/128", -1).cidr);
}
