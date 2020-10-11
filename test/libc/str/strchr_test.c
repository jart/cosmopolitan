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
#include "libc/bits/bits.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(strchr, blank) {
  const char *const blank = "";
  EXPECT_EQ(NULL, strchr(blank, '#'));
  EXPECT_EQ(blank, strchr(blank, '\0'));
}

TEST(strchr, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", strchr(buf, 't'));
}
TEST(strchr, testsse) {
  char buf[] = "hellohellohellohellohellohellohellohello"
               "theretheretheretheretheretheretherethere";
  EXPECT_STREQ("theretheretheretheretheretheretherethere", strchr(buf, 't'));
}
TEST(rawmemchr, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", rawmemchr(buf, 't'));
}
TEST(strchrnul, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", strchrnul(buf, 't'));
}

TEST(strchr, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", strchr(buf, '\0'));
}
TEST(rawmemchr, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", rawmemchr(buf, '\0'));
}
TEST(strchrnul, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", strchrnul(buf, '\0'));
}

TEST(strchr, notFound_returnsNul) {
  char buf[] = "hellothere";
  EXPECT_EQ(NULL, strchr(buf, 'z'));
}
TEST(strchrnul, notFound_returnsPointerToNulByte) {
  char buf[] = "hi";
  EXPECT_STREQ("", strchrnul(buf, 'z'));
  EXPECT_EQ(&buf[2], strchrnul(buf, 'z'));
}
