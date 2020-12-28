/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/bits.h"
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/testlib/testlib.h"

#define ROR(w, k) (CheckUnsigned(w) >> (k) | (w) << (sizeof(w) * 8 - (k)))
#define ROL(w, k) ((w) << (k) | CheckUnsigned(w) >> (sizeof(w) * 8 - (k)))

TEST(TwosComplementBane, LiteralsThatAreLiterallyTheSameNumber) {
  EXPECT_EQ(4, sizeof(INT_MIN));
  EXPECT_EQ(8, sizeof(-2147483648));
  EXPECT_TRUE(TYPE_SIGNED(-2147483648));
  EXPECT_FALSE(TYPE_SIGNED(0x80000000));
  EXPECT_FALSE(TYPE_SIGNED(-0x80000000));
}

TEST(RotateRight, Test) {
  EXPECT_EQ(0x41122334u, ROR(0x11223344u, 4));
  EXPECT_EQ(0x44112233u, ROR(0x11223344u, 8));
  EXPECT_EQ(0x34411223u, ROR(0x11223344u, 12));
  EXPECT_EQ(0x33441122u, ROR(0x11223344u, 16));
}

TEST(RotateLeft, Test) {
  EXPECT_EQ(0x12233441u, ROL(0x11223344u, 4));
  EXPECT_EQ(0x22334411u, ROL(0x11223344u, 8));
  EXPECT_EQ(0x23344112u, ROL(0x11223344u, 12));
  EXPECT_EQ(0x33441122u, ROL(0x11223344u, 16));
}
