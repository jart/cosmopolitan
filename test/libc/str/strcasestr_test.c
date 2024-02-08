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
#include "libc/dce.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/tab.internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

#define MAKESTRING(NAME, VALUE) \
  char *NAME = strcpy(malloc(sizeof(VALUE) + 16), VALUE)

char *strcasestr_naive(const char *haystack, const char *needle) {
  size_t i;
  if (haystack == needle || !*needle) return (void *)haystack;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (kToLower[needle[i] & 255] != kToLower[haystack[i] & 255]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
}

TEST(strcasestr, test_emptyString_isFoundAtBeginning) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[0], strcasestr(haystack, gc(strdup(""))));
  free(haystack);
}

TEST(strcasestr, test_notFound) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_EQ(NULL, strcasestr(haystack, gc(strdup("xyz"))));
  ASSERT_EQ(NULL, strcasestr(haystack, gc(strdup("XYZ"))));
  free(haystack);
}

TEST(strcasestr, test_notFound1) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_EQ(NULL, strcasestr(haystack, gc(strdup("x"))));
  ASSERT_EQ(NULL, strcasestr(haystack, gc(strdup("X"))));
  free(haystack);
}

TEST(strcasestr, test_middleOfString) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[3], strcasestr(haystack, gc(strdup("123"))));
  free(haystack);
}

TEST(strcasestr, test_endOfString) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[8], strcasestr(haystack, gc(strdup("f"))));
  ASSERT_STREQ(&haystack[8], strcasestr(haystack, gc(strdup("F"))));
  free(haystack);
}

TEST(strcasestr, test_secondXmmWord) {
  MAKESTRING(haystack, "eeeeeeeeeeeeeeeebbbbbbbbbbb123");
  ASSERT_STREQ(&haystack[27], strcasestr(haystack, gc(strdup("123"))));
  free(haystack);
}

TEST(strcasestr, test_overlapsXmmWords) {
  MAKESTRING(haystack, "eeeeeeeeeeeeeeeebbbbbbbbbbbbbbb");
  ASSERT_STREQ(&haystack[15], strcasestr(haystack, gc(strdup("eb"))));
  ASSERT_STREQ(&haystack[15], strcasestr(haystack, gc(strdup("Eb"))));
  ASSERT_STREQ(&haystack[15], strcasestr(haystack, gc(strdup("eB"))));
  free(haystack);
}

TEST(strcasestr, test) {
  ASSERT_EQ(NULL, strcasestr("x86_64-linux-musl-gcc", "clang"));
  ASSERT_STREQ("gcc", strcasestr("x86_64-linux-musl-gcc", "gcc"));
  ASSERT_EQ(NULL, strcasestr("-Wl,--gc-sections", "stack-protector"));
  ASSERT_EQ(NULL, strcasestr("-Wl,--gc-sections", "sanitize"));
  ASSERT_STREQ("x", strcasestr("x", "x"));
}

/*
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
BENCH(strcasestr, bench) {
  EZBENCH2("strcasestr naive", donothing,
           __expropriate(strcasestr_naive(kHyperion, "THE END")));
  EZBENCH2("strcasestr", donothing,
           __expropriate(strcasestr(kHyperion, "THE END")));
  EZBENCH2("strcasestr tort 1", donothing,
           __expropriate(strcasestr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "B")));
  EZBENCH2("strcasestr tort 2", donothing,
           __expropriate(strcasestr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "ab")));
  EZBENCH2("strcasestr tort 4", donothing,
           __expropriate(strcasestr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaab")));
  EZBENCH2("strcasestr tort 8", donothing,
           __expropriate(strcasestr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaab")));
  EZBENCH2("strcasestr tort 16", donothing,
           __expropriate(strcasestr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaaaaaaaaaab")));
  EZBENCH2("strcasestr tort 32", donothing,
           __expropriate(strcasestr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab")));
}
