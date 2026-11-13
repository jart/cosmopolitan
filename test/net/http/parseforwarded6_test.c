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
#include "net/http/http.h"
#include "net/http/ip.h"

static const uint8_t kDoc[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                 0,    0,    0,    0,    0, 0, 0, 0};

static const uint8_t kDoc1[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                  0,    0,    0,    0,    0, 0, 0, 1};

static const uint8_t kDoc2[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                  0,    0,    0,    0,    0, 0, 0, 2};

TEST(ParseForwarded6, test) {
  uint8_t ip[16];
  uint16_t port = 7;
  EXPECT_EQ(-1, ParseForwarded6("", -1, ip, &port));
  EXPECT_EQ(7, port);
  EXPECT_EQ(-1, ParseForwarded6("1.2.3.4", -1, ip, &port));
  EXPECT_EQ(0, ParseForwarded6("1.2.3.4, 2001:db8::1", -1, ip, &port));
  EXPECT_EQ(0, memcmp(ip, kDoc1, 16));
  EXPECT_EQ(-1, ParseForwarded6("2001:db8::1, 1.2.3.4", -1, ip, &port));
  EXPECT_EQ(0, ParseForwarded6("2001:db8::1", -1, ip, &port));
  EXPECT_EQ(0, port);
  EXPECT_EQ(0, memcmp(ip, kDoc1, 16));
  EXPECT_EQ(0, ParseForwarded6("[2001:db8::1]:8080", -1, ip, &port));
  EXPECT_EQ(8080, port);
  EXPECT_EQ(0, memcmp(ip, kDoc1, 16));
  EXPECT_EQ(0, ParseForwarded6("[2001:db8::1]", -1, ip, &port));
  EXPECT_EQ(0, port);
  EXPECT_EQ(0, memcmp(ip, kDoc1, 16));
  EXPECT_EQ(0, ParseForwarded6("[2001:db8::2]:443", -1, ip, &port));
  EXPECT_EQ(443, port);
  EXPECT_EQ(0, memcmp(ip, kDoc2, 16));
  EXPECT_EQ(0,
            ParseForwarded6("203.0.113.1, [2001:db8::2]:443", -1, ip, &port));
  EXPECT_EQ(443, port);
  EXPECT_EQ(0, memcmp(ip, kDoc2, 16));
  EXPECT_EQ(0, ParseForwarded6("2001:db8::", -1, ip, &port));
  EXPECT_EQ(0, memcmp(ip, kDoc, 16));
}

TEST(ParseForwarded6, bad) {
  uint8_t ip[16];
  uint16_t port;
  EXPECT_EQ(-1, ParseForwarded6(":::", -1, ip, &port));
  EXPECT_EQ(-1, ParseForwarded6("[::1", -1, ip, &port));
  EXPECT_EQ(-1, ParseForwarded6("[::1]x", -1, ip, &port));
  EXPECT_EQ(0, ParseForwarded6("[::1]:", -1, ip, &port));
  EXPECT_EQ(0, port);
  EXPECT_EQ(-1, ParseForwarded6("[::1]:65536", -1, ip, &port));
  EXPECT_EQ(-1, ParseForwarded6("[::1]:80x", -1, ip, &port));
  EXPECT_EQ(-1, ParseForwarded6("::1%eth0", -1, ip, &port));
}

TEST(ParseForwarded6, nullable) {
  uint8_t ip[16];
  uint16_t port;
  EXPECT_EQ(0, ParseForwarded6("2001:db8::1", -1, 0, 0));
  EXPECT_EQ(0, ParseForwarded6("[2001:db8::1]:80", -1, ip, 0));
  EXPECT_EQ(0, ParseForwarded6("[2001:db8::1]:80", -1, 0, &port));
  EXPECT_EQ(80, port);
}
