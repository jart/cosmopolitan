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

TEST(T(strrchr), test) {
  EXPECT_EQ(NULL, strrchr(S("hello"), C('z')));
  EXPECT_STREQ(S("lo"), strrchr(S("hello"), C('l')));
  EXPECT_STREQ(S("llo"), strchr(S("hello"), C('l')));
  EXPECT_STREQ(S("hello"), strrchr(S("hello"), C('h')));
  EXPECT_STREQ(S("ello"), strrchr(S("hello"), C('e')));
  EXPECT_STREQ(S("o"), strrchr(S("hello"), C('o')));
}

TEST(T(strrchr), simdVectorStuffIsntBroken) {
  EXPECT_EQ(NULL, strrchr(S("--------------------------------"), C('x')));
  EXPECT_STREQ(S("x"), strrchr(S("-------------------------------x"), C('x')));
  EXPECT_STREQ(S("x-------------------------------"),
               strrchr(S("x-------------------------------"), C('x')));
  EXPECT_STREQ(S("x") S("z-------------------------------"),
               strrchr(S("x") S("z-------------------------------"), C('x')));
  EXPECT_STREQ(S("x-------------------------------")
                   S("y-------------------------------"),
               strrchr(S("x-------------------------------")
                           S("y-------------------------------"),
                       C('x')));
  EXPECT_STREQ(S("x") S("z-------------------------------")
                   S("y-------------------------------"),
               strrchr(S("x") S("z-------------------------------")
                           S("y-------------------------------"),
                       C('x')));
}
