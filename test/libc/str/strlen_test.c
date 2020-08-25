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
#include "libc/macros.h"
#include "libc/nexgen32e/tinystrlen.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

char u8[] = "utf-8 ☻";
char16_t u16[] = u"utf16 ☻";
wchar_t u32[] = L"utf32 ☻";

TEST(strlen, usageExample_c11) {
  EXPECT_EQ(6 + 3, strlen(u8));
  EXPECT_EQ(7, strlen(u16));
  EXPECT_EQ(7, strlen(u32));
}

TEST(strlen, usageExample_c99) {
  EXPECT_EQ(6 + 3, strlen(u8));
  EXPECT_EQ(7, strlen16(u16));
  EXPECT_EQ(7, wcslen(u32));
}

TEST(strlen, whatMemoryLooksLike) {
  EXPECT_BINEQ(u"utf-8 Γÿ╗ ", u8); /* ← thompson-pike encoded */
  EXPECT_BINEQ(u"u t f 1 6   ;&  ", u16);
  EXPECT_BINEQ(u"u   t   f   3   2       ;&      ", u32);
}

TEST(strlen, test_const) {
  const char buf[] = "hellothere";
  ASSERT_EQ(0, strlen(""));
  ASSERT_EQ(0, strnlen("e", 0));
  ASSERT_EQ(10, strlen(buf));
}

TEST(strlen, test_nonconst) {
  char buf[256];
  unsigned i;
  for (i = 0; i < 255; ++i) buf[i] = i + 1;
  buf[i] = '\0';
  ASSERT_EQ(255, strlen(buf));
}

TEST(strnlen, testconst) {
  ASSERT_EQ(0, strnlen_s(NULL, 3));
  ASSERT_EQ(0, strnlen("", 3));
  ASSERT_EQ(0, strnlen("a", 0));
  ASSERT_EQ(3, strnlen("123", 3));
  ASSERT_EQ(2, strnlen("123", 2));
  ASSERT_EQ(3, strnlen("123", 4));
}

TEST(strlen, testnonconst) {
  char buf[256];
  unsigned i;
  for (i = 0; i < 255; ++i) buf[i] = i + 1;
  buf[i] = '\0';
  ASSERT_EQ(255, strlen(buf));
}

TEST(strnlen_s, null_ReturnsZero) {
  ASSERT_EQ(0, strnlen_s(NULL, 3));
}

TEST(strnlen, nulNotFound_ReturnsSize) {
  int sizes[] = {1, 2, 7, 8, 15, 16, 31, 32, 33};
  for (unsigned i = 0; i < ARRAYLEN(sizes); ++i) {
    char *buf = tmalloc(sizes[i]);
    memset(buf, ' ', sizes[i]);
    ASSERT_EQ(sizes[i], strnlen(buf, sizes[i]), "%d", sizes[i]);
    tfree(buf);
  }
}

TEST(strnlen_s, nulNotFound_ReturnsZero) {
  char buf[3] = {1, 2, 3};
  ASSERT_EQ(0, strnlen_s(buf, 3));
}

TEST(tinystrlen, test) {
  ASSERT_EQ(0, tinystrlen(""));
  ASSERT_EQ(1, tinystrlen("a"));
  ASSERT_EQ(3, tinystrlen("123"));
}

TEST(tinywcslen, test) {
  ASSERT_EQ(0, tinywcslen(L""));
  ASSERT_EQ(1, tinywcslen(L"a"));
  ASSERT_EQ(3, tinywcslen(L"123"));
}

TEST(tinywcsnlen, test) {
  EXPECT_EQ(0, tinywcsnlen(L"", 3));
  EXPECT_EQ(0, tinywcsnlen(L"a", 0));
  EXPECT_EQ(3, tinywcsnlen(L"123", 3));
  EXPECT_EQ(2, tinywcsnlen(L"123", 2));
  EXPECT_EQ(3, tinywcsnlen(L"123", 4));
}

TEST(tinystrlen16, test) {
  ASSERT_EQ(0, tinystrlen16(u""));
  ASSERT_EQ(1, tinystrlen16(u"a"));
  ASSERT_EQ(3, tinystrlen16(u"123"));
}

TEST(tinystrnlen16, test) {
  EXPECT_EQ(0, tinystrnlen16(u"", 3));
  EXPECT_EQ(0, tinystrnlen16(u"a", 0));
  EXPECT_EQ(3, tinystrnlen16(u"123", 3));
  EXPECT_EQ(2, tinystrnlen16(u"123", 2));
  EXPECT_EQ(3, tinystrnlen16(u"123", 4));
}
