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
#include "libc/rand/rand.h"
#include "libc/testlib/testlib.h"

TEST(rand002, alwaysReturnsPositiveNumbers) {
  for (unsigned i = 0; i < 100; ++i) {
    ASSERT_GT(rand(), 0);
  }
}

TEST(rand003, srandSmokeTest) {
  srand(1);
  ASSERT_EQ(908834774, rand());
  srand(1);
  ASSERT_EQ(908834774, rand());
  srand(7);
  ASSERT_EQ(1059165278, rand());
}

TEST(rand004, rand32SmokeTest) {
  ASSERT_TRUE(rand32() != rand32() || rand32() != rand32() ||
              rand32() != rand32() || rand32() != rand32());
}

TEST(rand005, rand64SmokeTest) {
  ASSERT_TRUE(rand64() != rand64() || rand64() != rand64());
}
