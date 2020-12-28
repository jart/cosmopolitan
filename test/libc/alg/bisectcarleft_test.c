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
#include "libc/alg/alg.h"
#include "libc/alg/bisectcarleft.internal.h"
#include "libc/bits/bits.h"
#include "libc/macros.h"
#include "libc/runtime/runtime.h"
#include "libc/testlib/testlib.h"

TEST(bisectcarleft, testEmpty) {
  const int32_t cells[][2] = {};
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), 123));
}

TEST(bisectcarleft, testOneEntry) {
  const int32_t cells[][2] = {{123, 31337}};
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), 122));
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), 123));
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), 124));
}

TEST(bisectcarleft, testNegativity_usesSignedBehavior) {
  const int32_t cells[][2] = {{-2, 31337}};
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), -3));
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), -2));
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), -1));
}

TEST(bisectcarleft, testMultipleEntries) {
  const int32_t cells[][2] = {{00, 0}, {11, 0}, {20, 0}, {33, 0}, {40, 0},
                              {50, 0}, {60, 0}, {70, 0}, {80, 0}, {90, 0}};
  EXPECT_EQ(0, bisectcarleft(cells, ARRAYLEN(cells), 10));
  EXPECT_EQ(1, bisectcarleft(cells, ARRAYLEN(cells), 11));
  EXPECT_EQ(1, bisectcarleft(cells, ARRAYLEN(cells), 12));
  EXPECT_EQ(1, bisectcarleft(cells, ARRAYLEN(cells), 19));
  EXPECT_EQ(2, bisectcarleft(cells, ARRAYLEN(cells), 20));
  EXPECT_EQ(2, bisectcarleft(cells, ARRAYLEN(cells), 21));
  EXPECT_EQ(2, bisectcarleft(cells, ARRAYLEN(cells), 32));
  EXPECT_EQ(3, bisectcarleft(cells, ARRAYLEN(cells), 33));
  EXPECT_EQ(3, bisectcarleft(cells, ARRAYLEN(cells), 34));
}
