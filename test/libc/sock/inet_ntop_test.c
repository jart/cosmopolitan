/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/af.h"
#include "libc/testlib/testlib.h"

TEST(inet_ntop, test) {
  char buf[16];
  uint8_t localhost[4] = {127, 0, 0, 1};
  EXPECT_STREQ("127.0.0.1", inet_ntop(AF_INET, localhost, buf, sizeof(buf)));
}

TEST(inet_ntop, testMax) {
  char buf[16];
  uint8_t localhost[4] = {255, 255, 255, 255};
  EXPECT_STREQ("255.255.255.255",
               inet_ntop(AF_INET, localhost, buf, sizeof(buf)));
}

TEST(inet_ntop, testBadFamily) {
  char buf[16] = "hi";
  uint8_t localhost[4] = {127, 0, 0, 1};
  ASSERT_EQ(NULL, inet_ntop(666, localhost, buf, sizeof(buf)));
  EXPECT_EQ(EAFNOSUPPORT, errno);
  ASSERT_STREQ("hi", buf);
}

TEST(inet_ntop, testNoSpace) {
  char *buf = memcpy(malloc(16), "hi", 3);
  uint8_t localhost[4] = {127, 0, 0, 1};
  EXPECT_EQ(NULL, inet_ntop(AF_INET, localhost, buf, 1));
  EXPECT_EQ(ENOSPC, errno);
  ASSERT_STREQ("hi", buf);
  ASSERT_EQ(NULL, inet_ntop(AF_INET, localhost, buf, 7));
  ASSERT_STREQ("hi", buf);
  free(buf);
}

TEST(inet_ntop, ipv6_testMin_isJustColons) {
  char buf[46];
  uint8_t ip[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  EXPECT_STREQ("::", inet_ntop(AF_INET6, ip, buf, sizeof(buf)));
}

TEST(inet_ntop, ipv6_testMax) {
  char buf[46];
  uint8_t ip[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  EXPECT_STREQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
               inet_ntop(AF_INET6, ip, buf, sizeof(buf)));
}

TEST(inet_ntop, ipv6_loopback_isColonsThenJustOne) {
  char buf[46];
  uint8_t ip[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  EXPECT_STREQ("::1", inet_ntop(AF_INET6, ip, buf, sizeof(buf)));
}

TEST(inet_ntop, ipv6_rfc4291example) {
  char buf[46];
  uint8_t ip[16] = {0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x08, 0x08, 0x00, 0x20, 0x0C, 0x41, 0x7A};
  EXPECT_STREQ("2001:db8::8:800:200c:417a",
               inet_ntop(AF_INET6, ip, buf, sizeof(buf)));
}

TEST(inet_ntop, ipv6_leading) {
  char buf[46];
  uint8_t ip[16] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  EXPECT_STREQ("1::", inet_ntop(AF_INET6, ip, buf, sizeof(buf)));
}

TEST(inet_ntop, ipv6_kindOfLeading) {
  char buf[46];
  uint8_t ip[16] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  EXPECT_STREQ("100::", inet_ntop(AF_INET6, ip, buf, sizeof(buf)));
}
