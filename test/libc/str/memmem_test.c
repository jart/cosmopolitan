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
#include "libc/mem/mem.h"
#include "libc/intrin/likely.h"
#include "libc/mem/alg.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

#define MakeMemory(SL) memcpy(malloc(sizeof(SL) - 1), SL, sizeof(SL) - 1)

void *memmem_naive(const void *haystk, size_t haystklen,  //
                   const void *needle, size_t needlelen) {
  size_t i, j;
  if (!needlelen) return (void *)haystk;
  if (needlelen > haystklen) return 0;
  for (i = 0; i < haystklen; ++i) {
    for (j = 0;; ++j) {
      if (j == needlelen) return (/*unconst*/ char *)haystk + i;
      if (i + j == haystklen) break;
      if (((char *)haystk)[i + j] != ((char *)needle)[j]) break;
    }
  }
  return 0;
}

TEST(memmem, special) {
  EXPECT_EQ(0, memmem(0, 0, 0, 0));
  EXPECT_EQ(0, memmem(0, 0, "", 1));
  EXPECT_EQ("", memmem("", 1, 0, 0));
}

TEST(memmem, test) {
  char *needle = MakeMemory("abcdefgh");
  char *haystk = MakeMemory("acccccccbbbbbbbbabcdefghdddddddd");
  EXPECT_BINEQ(u"abcdefghdddddddd", memmem(haystk, 32, needle, 8));
  memcpy(needle, "aaaaaaaa", 8);
  memcpy(haystk, "acccccccbbbbbbbbaaaaaaaadddddddd", 32);
  EXPECT_BINEQ(u"aaaaaaaadddddddd", memmem(haystk, 32, needle, 8));
  free(haystk);
  free(needle);
}

TEST(memmem, testNoMatch) {
  char *needle = MakeMemory("abcdefzh");
  char *haystk = MakeMemory("acccccccbbbbbbbbabcdefghdddddddd");
  EXPECT_EQ(NULL, memmem(haystk, 32, needle, 8));
  free(haystk);
  free(needle);
}

TEST(memmem, testStartOfMemory) {
  char *needle = MakeMemory("acccc");
  char *haystk = MakeMemory("acccccccbbbbbbbbabcdefghdddddddd");
  EXPECT_EQ(&haystk[0], memmem(haystk, 32, needle, 5));
  free(haystk);
  free(needle);
}

TEST(memmem, testEndOfMemory) {
  char *haystk = MakeMemory("abc123");
  char *needle = MakeMemory("123");
  EXPECT_EQ(&haystk[3], memmem(haystk, 6, needle, 3));
  free(haystk);
  free(needle);
}

TEST(memmem, testOneNo) {
  char *needle = MakeMemory("z");
  char *haystk = MakeMemory("abc123");
  EXPECT_EQ(0, memmem(haystk, 6, needle, 1));
  free(haystk);
  free(needle);
}

TEST(memmem, testOneYes) {
  char *needle = MakeMemory("3");
  char *haystk = MakeMemory("abc123");
  EXPECT_EQ(&haystk[5], memmem(haystk, 6, needle, 1));
  free(haystk);
  free(needle);
}

TEST(memmem, testCrossesSseRegister) {
  char *haystk = MakeMemory("eeeeeeeeeeeeeeeeffffffffffffffffrrrrrrrrrrrrrrrr");
  char *needle = MakeMemory("eeeeeeeeeeeeefffffffffffff");
  EXPECT_EQ(&haystk[3], memmem(haystk, 16 * 3, needle, 26));
  free(haystk);
  free(needle);
}

TEST(memmem, testHasNulCharacters) {
  char *needle = MakeMemory("eeeeeeeeeeeee\0ffffffffffff");
  char *haystk =
      MakeMemory("eeeeeeeeeeeeeeee\0fffffffffffffffrrrrrrrrrrrrrrrr");
  EXPECT_EQ(&haystk[3], memmem(haystk, 16 * 3, needle, 26));
  free(haystk);
  free(needle);
}

TEST(memmem, testWeird) {
  char *needle = MakeMemory("-*-+-+-+-+-+-+-+");
  char *haystk = MakeMemory("-+-+-+-+-+-+-+-*-+-+-+-+-+-+-+-+");
  EXPECT_EQ(14, (intptr_t)memmem(haystk, 32, needle, 16) - (intptr_t)haystk);
  free(haystk);
  free(needle);
}

TEST(memmem, testEmptyNeedle_matchesStartOfHaystack) {
  char *needle = malloc(0);
  char *haystk = MakeMemory("-+-+-+-+-+-+-+-*-+-+-+-+-+-+-+-+");
  EXPECT_EQ(0, (intptr_t)memmem(haystk, 32, needle, 0) - (intptr_t)haystk);
  free(haystk);
  free(needle);
}

TEST(memmem, testEmptyHaystack_alwaysReturnsNull) {
  char *needle = MakeMemory("-*-+-+-+-+-+-+-+");
  char *haystk = malloc(0);
  EXPECT_EQ(NULL, memmem(haystk, 0, needle, 16));
  EXPECT_EQ(NULL, memmem(haystk, 0, needle, 1));
  free(haystk);
  free(needle);
}

TEST(memmem, testEmptyHaystackAndNeedle_returnsHaystack) {
  char *needle = malloc(0);
  char *haystk = malloc(0);
  EXPECT_EQ(haystk, memmem(haystk, 0, needle, 0));
  free(haystk);
  free(needle);
}

TEST(memmem, testWut) {
  ASSERT_STREQ("x", memmem("x", 1, "x", 1));
}

TEST(memmem, fuzz) {
  int i, n, m;
  char a[128], b[128], *p, *q;
  for (i = 0; i < 10000; ++i) {
    rngset(a, sizeof(a), lemur64, -1);
    rngset(b, sizeof(b), lemur64, -1);
    p = a + lemur64() % sizeof(a) / 2;
    q = b + lemur64() % sizeof(b) / 2;
    n = lemur64() % sizeof(a) / 2;
    m = lemur64() % sizeof(b) / 2;
    ASSERT_EQ(memmem_naive(p, n, q, m), memmem(p, n, q, m));
  }
}

/*
 *     memmem naive        l:    43,783c    14,142ns   m:    31,285c    10,105ns
 *     memmem              l:     2,597c       839ns   m:     2,612c       844ns
 *     memmem              l:       509c       164ns   m:       599c       193ns
 *
 *     strstr naive        l:   103,057c    33,287ns   m:    47,035c    15,192ns
 *     strstr              l:     3,186c     1,029ns   m:     3,218c     1,039ns
 *     strstr torture 1    l:        27c         9ns   m:        61c        20ns
 *     strstr torture 2    l:     2,322c       750ns   m:     2,362c       763ns
 *     strstr torture 4    l:     2,407c       777ns   m:     2,448c       791ns
 *     strstr torture 8    l:     2,803c       905ns   m:     2,862c       924ns
 *     strstr torture 16   l:     4,559c     1,473ns   m:     3,614c     1,167ns
 *     strstr torture 32   l:     5,324c     1,720ns   m:     5,577c     1,801ns
 *
 *     strcasestr naive    l:   129,908c    41,959ns   m:   155,420c    50,200ns
 *     strcasestr          l:    33,464c    10,809ns   m:    31,636c    10,218ns
 *     strcasestr tort 1   l:        38c        12ns   m:        69c        22ns
 *     strcasestr tort 2   l:     2,544c       822ns   m:     2,580c       833ns
 *     strcasestr tort 4   l:     2,745c       887ns   m:     2,767c       894ns
 *     strcasestr tort 8   l:     4,198c     1,356ns   m:     4,216c     1,362ns
 *     strcasestr tort 16  l:     7,402c     2,391ns   m:     7,487c     2,418ns
 *     strcasestr tort 32  l:    13,772c     4,448ns   m:    12,945c     4,181ns
 */
BENCH(memmem, bench) {
  EZBENCH2("memmem naive", donothing,
           __expropriate(memmem_naive(kHyperion, kHyperionSize, "THE END", 7)));
  EZBENCH2("memmem", donothing,
           __expropriate(memmem(kHyperion, kHyperionSize, "THE END", 7)));
  EZBENCH2("memmem", donothing,
           __expropriate(memmem(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               62, "aaaaaab", 7)));
}
