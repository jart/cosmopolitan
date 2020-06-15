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
#include "libc/alg/alg.h"
#include "libc/alg/bisectcarleft.h"
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
