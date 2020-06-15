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
#include "libc/bits/bits.h"
#include "libc/str/internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

void *(*memmemi)(const void *, size_t, const void *, size_t) = memmem;
FIXTURE(memmem, tiny) {
  memmemi = tinymemmem;
}

#define MakeMemory(SL) memcpy(tmalloc(sizeof(SL) - 1), SL, sizeof(SL) - 1)

TEST(memmem, test) {
  char *needle = MakeMemory("abcdefgh");
  char *haystk = MakeMemory("acccccccbbbbbbbbabcdefghdddddddd");
  EXPECT_BINEQ(u"abcdefghdddddddd", memmemi(haystk, 32, needle, 8));
  memcpy(needle, "aaaaaaaa", 8);
  memcpy(haystk, "acccccccbbbbbbbbaaaaaaaadddddddd", 32);
  EXPECT_BINEQ(u"aaaaaaaadddddddd", memmemi(haystk, 32, needle, 8));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testNoMatch) {
  char *needle = MakeMemory("abcdefzh");
  char *haystk = MakeMemory("acccccccbbbbbbbbabcdefghdddddddd");
  EXPECT_EQ(NULL, memmemi(haystk, 32, needle, 8));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testStartOfMemory) {
  char *needle = MakeMemory("acccc");
  char *haystk = MakeMemory("acccccccbbbbbbbbabcdefghdddddddd");
  EXPECT_EQ(&haystk[0], memmemi(haystk, 32, needle, 5));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testEndOfMemory) {
  char *needle = MakeMemory("123");
  char *haystk = MakeMemory("abc123");
  EXPECT_EQ(&haystk[3], memmemi(haystk, 6, needle, 3));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testCrossesSseRegister) {
  char *needle = MakeMemory("eeeeeeeeeeeeefffffffffffff");
  char *haystk = MakeMemory("eeeeeeeeeeeeeeeeffffffffffffffffrrrrrrrrrrrrrrrr");
  EXPECT_EQ(&haystk[3], memmemi(haystk, 16 * 3, needle, 26));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testHasNulCharacters) {
  char *needle = MakeMemory("eeeeeeeeeeeee\0ffffffffffff");
  char *haystk =
      MakeMemory("eeeeeeeeeeeeeeee\0fffffffffffffffrrrrrrrrrrrrrrrr");
  EXPECT_EQ(&haystk[3], memmemi(haystk, 16 * 3, needle, 26));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testWeird) {
  char *needle = MakeMemory("-*-+-+-+-+-+-+-+");
  char *haystk = MakeMemory("-+-+-+-+-+-+-+-*-+-+-+-+-+-+-+-+");
  EXPECT_EQ(14, (intptr_t)memmemi(haystk, 32, needle, 16) - (intptr_t)haystk);
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testEmptyNeedle_matchesStartOfHaystack) {
  char *needle = tmalloc(0);
  char *haystk = MakeMemory("-+-+-+-+-+-+-+-*-+-+-+-+-+-+-+-+");
  EXPECT_EQ(0, (intptr_t)memmemi(haystk, 32, needle, 0) - (intptr_t)haystk);
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testEmptyHaystack_alwaysReturnsNull) {
  char *needle = MakeMemory("-*-+-+-+-+-+-+-+");
  char *haystk = tmalloc(0);
  EXPECT_EQ(NULL, memmemi(haystk, 0, needle, 16));
  EXPECT_EQ(NULL, memmemi(haystk, 0, needle, 1));
  tfree(haystk);
  tfree(needle);
}

TEST(memmem, testEmptyHaystackAndNeedle_returnsHaystack) {
  char *needle = tmalloc(0);
  char *haystk = tmalloc(0);
  EXPECT_EQ(haystk, memmemi(haystk, 0, needle, 0));
  tfree(haystk);
  tfree(needle);
}
