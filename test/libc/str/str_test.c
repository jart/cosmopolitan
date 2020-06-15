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

TEST(strlen16, testEmpty) { EXPECT_EQ(0, strlen(u"")); }
TEST(strlen16, testAscii) { EXPECT_EQ(5, strlen(u"hello")); }

TEST(strlen16, testUnicode) {
  EXPECT_EQ(28, strlen(u"αcτµαlly pδrταblε εxεcµταblε"));
}

TEST(strclen, testAegeanNumberSupplementaryPlane) {
  EXPECT_EQ(36, strlen("𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(18, strlen(u"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(9, strlen(L"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(9, strclen("𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(9, strclen(u"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(9, strclen(L"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
}

TEST(strlen16, testCoolKidNulTerminator) {
  EXPECT_EQ(2, strlen((const char16_t *)"\x00\xd8\x00\xdc\x00"));
}
