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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(strlcpy, testEmptyBuffer_doesNothing) {
  EXPECT_EQ(5, strlcpy(NULL, "hello", 0));
}

TEST(strlcpy, testShortBuffer_nulTerminates) {
  char buf[1] = {1};
  EXPECT_EQ(5, strlcpy(buf, "hello", 1));
  EXPECT_STREQ("", buf);
}

TEST(strlcpy, testShortBuffer_copies) {
  char buf[2] = {1, 1};
  EXPECT_EQ(5, strlcpy(buf, "hello", 2));
  EXPECT_STREQ("h", buf);
}
