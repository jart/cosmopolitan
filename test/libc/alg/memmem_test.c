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
