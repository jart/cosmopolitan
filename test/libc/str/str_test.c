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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(strlen16, testEmpty) {
  EXPECT_EQ(0, strlen16(u""));
}
TEST(strlen16, testAscii) {
  EXPECT_EQ(5, strlen16(u"hello"));
}

TEST(strlen16, testUnicode) {
  EXPECT_EQ(28, strlen16(u"αcτµαlly pδrταblε εxεcµταblε"));
}

TEST(len, testAegeanNumberSupplementaryPlane) {
  EXPECT_EQ(36, strlen("𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(18, strlen16(u"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(9, wcslen(L"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
  EXPECT_EQ(9, wcslen(L"𐄷𐄸𐄹𐄺𐄻𐄼𐄽𐄾𐄿"));
}

TEST(strlen16, testCoolKidNulTerminator) {
  union {
    uint8_t s8[6];
    char16_t s16[3];
  } u = {.s8 = {0x00, 0xd8, 0x00, 0xdc, 0x00, 0x00}};
  EXPECT_EQ(2, strlen16(u.s16));
}
