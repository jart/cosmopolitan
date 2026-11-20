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
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "net/http/ip.h"

static const uint8_t kZero[16] = {0};
static const uint8_t kLoopback[16] = {0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 1};
static const uint8_t kDoc[16] = {0x20, 0x01, 0x0d, 0xb8, 0, 0, 0, 0,
                                 0,    0,    0,    0,    0, 0, 0, 1};
static const uint8_t kMapped[16] = {0, 0, 0,    0,    0, 0, 0, 0,
                                    0, 0, 0xff, 0xff, 1, 2, 3, 4};
static const uint8_t kMapped34[16] = {0, 0, 0,    0,    0, 0, 0, 0,
                                      0, 0, 0xff, 0xff, 3, 4, 5, 6};

static bool Ok(const char *s, const uint8_t *want) {
  uint8_t b[16];
  return !ParseIp6(s, -1, b) && !memcmp(b, want, 16);
}

static bool OkN(const char *s, size_t n, const uint8_t *want) {
  uint8_t b[16];
  return !ParseIp6(s, n, b) && !memcmp(b, want, 16);
}

TEST(ParseIp6, test) {
  uint8_t b[16];
  EXPECT_TRUE(Ok("::", kZero));
  EXPECT_TRUE(Ok("::1", kLoopback));
  EXPECT_TRUE(Ok("0:0:0:0:0:0:0:1", kLoopback));
  EXPECT_TRUE(Ok("2001:db8::1", kDoc));
  EXPECT_TRUE(Ok("2001:0db8:0000:0000:0000:0000:0000:0001", kDoc));
  EXPECT_TRUE(Ok("2001:db8:0:0:0:0:0:1", kDoc));
  EXPECT_TRUE(Ok("::ffff:1.2.3.4", kMapped));
  EXPECT_TRUE(Ok("::ffff:0102:0304", kMapped));
  EXPECT_TRUE(Ok("0:0:0:0:0:ffff:102:304", kMapped));
  EXPECT_TRUE(Ok("::FFFF:3.4.5.6", kMapped34));
  EXPECT_EQ(0, ParseIp6("1:2:3:4:5:6:7:8", -1, b));
  EXPECT_EQ(0, ParseIp6("1::", -1, b));
  EXPECT_EQ(0, ParseIp6("::1.2.3.4", -1, b));
}

TEST(ParseIp6, bad) {
  uint8_t b[16];
  EXPECT_EQ(-1, ParseIp6("", -1, b));
  EXPECT_EQ(-1, ParseIp6(0, -1, b));
  EXPECT_EQ(-1, ParseIp6(":::", -1, b));
  EXPECT_EQ(-1, ParseIp6(":::", 3, b));
  EXPECT_EQ(-1, ParseIp6("1::2::3", -1, b));
  EXPECT_EQ(-1, ParseIp6("12345::", -1, b));
  EXPECT_EQ(-1, ParseIp6("1:2:3:4:5:6:7:8:9", -1, b));
  EXPECT_EQ(-1, ParseIp6("::g", -1, b));
  EXPECT_EQ(-1, ParseIp6("::1 ", -1, b));
  EXPECT_EQ(-1, ParseIp6(" ::1", -1, b));
  EXPECT_EQ(-1, ParseIp6("::1%eth0", -1, b));
  EXPECT_EQ(-1, ParseIp6("[::1]", -1, b));
  EXPECT_EQ(-1, ParseIp6("1.2.3.4", -1, b));
  EXPECT_EQ(-1, ParseIp6("::ffff:1..3.4", -1, b));
  EXPECT_EQ(-1, ParseIp6("::ffff:1.2.3.", -1, b));
  EXPECT_EQ(-1, ParseIp6("::ffff:1.2.3.256", -1, b));
  EXPECT_EQ(-1, ParseIp6("::ffff:1.2.3.04", -1, b));
  EXPECT_EQ(-1, ParseIp6("::ffff:01.2.3.4", -1, b));
  EXPECT_EQ(-1, ParseIp6("::ffff:1.2.3.4:5", -1, b));
  EXPECT_EQ(-1, ParseIp6("1:", -1, b));
  EXPECT_EQ(-1, ParseIp6(":1", -1, b));
}

TEST(ParseIp6, partialLength) {
  uint8_t b[16];
  EXPECT_TRUE(OkN("::1garbage", 3, kLoopback));
  EXPECT_TRUE(OkN("2001:db8::1", 11, kDoc));
  EXPECT_EQ(-1, ParseIp6("::1", 1, b));
  EXPECT_EQ(-1, ParseIp6("::1\0garbage", 11, b));
  EXPECT_EQ(-1, ParseIp6("2001:db8::1", 12, b));
}

TEST(ParseIp6, tooLong) {
  char buf[100];
  uint8_t b[16];
  memset(buf, 'a', sizeof(buf));
  EXPECT_EQ(-1, ParseIp6(buf, sizeof(buf), b));
}
