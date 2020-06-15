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
#include "libc/bits/progn.h"
#include "libc/bits/safemacros.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"
#include "libc/testlib/testlib.h"

TEST(inet_pton, testLocalhost) {
  uint32_t addr;
  EXPECT_EQ(htonl(INADDR_LOOPBACK),
            PROGN(ASSERT_EQ(1, inet_pton(AF_INET, "127.0.0.1", &addr)), addr));
}

TEST(inet_pton, testBadAddresses) {
  uint32_t addr;
  ASSERT_EQ(0, inet_pton(AF_INET, "127.0.0", &addr));
  ASSERT_EQ(0, inet_pton(AF_INET, "256.0.0.1", &addr));
}

TEST(inet_pton, testBadFamily) {
  uint32_t addr = 666;
  ASSERT_EQ(-1, inet_pton(666, "127.0.0.1", &addr));
}
