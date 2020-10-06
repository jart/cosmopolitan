/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/errno.h"
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
  ASSERT_STREQ("", buf);
}

TEST(inet_ntop, testNoSpace) {
  char *buf = memcpy(tmalloc(16), "hi", 3);
  uint8_t localhost[4] = {127, 0, 0, 1};
  ASSERT_EQ(NULL, inet_ntop(AF_INET, localhost, buf, 0));
  EXPECT_EQ(ENOSPC, errno);
  ASSERT_STREQ("hi", buf);
  ASSERT_EQ(NULL, inet_ntop(AF_INET, localhost, buf, 7));
  ASSERT_STREQ("", buf);
  tfree(buf);
}
