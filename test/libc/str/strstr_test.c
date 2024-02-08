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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

#define MAKESTRING(NAME, VALUE) \
  char *NAME = strcpy(malloc(sizeof(VALUE) + 16), VALUE)

char *strstr_naive(const char *haystack, const char *needle) {
  size_t i;
  if (haystack == needle || !*needle) return (void *)haystack;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (needle[i] != haystack[i]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
}

TEST(strstr, test_emptyString_isFoundAtBeginning) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[0], strstr(haystack, gc(strdup(""))));
  free(haystack);
}

TEST(strstr, test_notFound) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_EQ(NULL, strstr(haystack, gc(strdup("xyz"))));
  free(haystack);
}

TEST(strstr, test_notFound1) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_EQ(NULL, strstr(haystack, gc(strdup("x"))));
  free(haystack);
}

TEST(strstr, test_middleOfString) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[3], strstr(haystack, gc(strdup("123"))));
  free(haystack);
}

TEST(strstr, test_endOfString) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[8], strstr(haystack, gc(strdup("f"))));
  free(haystack);
}

TEST(strstr, test_secondXmmWord) {
  MAKESTRING(haystack, "eeeeeeeeeeeeeeeebbbbbbbbbbb123");
  ASSERT_STREQ(&haystack[27], strstr(haystack, gc(strdup("123"))));
  free(haystack);
}

TEST(strstr, test_overlapsXmmWords) {
  MAKESTRING(haystack, "eeeeeeeeeeeeeeeebbbbbbbbbbbbbbb");
  ASSERT_STREQ(&haystack[15], strstr(haystack, gc(strdup("eb"))));
  free(haystack);
}

TEST(strstr, test) {
  ASSERT_EQ(NULL, strstr("x86_64-linux-musl-gcc", "clang"));
  ASSERT_STREQ("gcc", strstr("x86_64-linux-musl-gcc", "gcc"));
  ASSERT_EQ(NULL, strstr("-Wl,--gc-sections", "stack-protector"));
  ASSERT_EQ(NULL, strstr("-Wl,--gc-sections", "sanitize"));
  ASSERT_STREQ("x", strstr("x", "x"));
}

TEST(strstr, breakit) {
  char *p;
  p = gc(calloc(1, 32));
  p[0] = 'c';
  p[1] = 'c';
  p[10] = 'b';
  ASSERT_EQ(NULL, strstr(p, "b"));
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
BENCH(strstr, bench) {
  EZBENCH2("strstr naive", donothing,
           __expropriate(strstr_naive(kHyperion, "THE END")));
  EZBENCH2("strstr", donothing, __expropriate(strstr(kHyperion, "THE END")));
  EZBENCH2("strstr torture 1", donothing,
           __expropriate(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "b")));
  EZBENCH2("strstr torture 2", donothing,
           __expropriate(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "ab")));
  EZBENCH2("strstr torture 4", donothing,
           __expropriate(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaab")));
  EZBENCH2("strstr torture 8", donothing,
           __expropriate(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaab")));
  EZBENCH2("strstr torture 16", donothing,
           __expropriate(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaaaaaaaaaab")));
  EZBENCH2("strstr torture 32", donothing,
           __expropriate(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab")));
}
