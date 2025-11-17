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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/safemacros.h"
#include "libc/mem/alg.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/sysconf.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/stdio/sysparam.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define MAKESTRING(NAME, VALUE) \
  wchar_t *NAME = wcscpy(malloc(4 * (sizeof(VALUE) + 16)), VALUE)

wchar_t *wcsstr_naive(const wchar_t *haystack, const wchar_t *needle) {
  size_t i;
  if (haystack == needle || !*needle)
    return (void *)haystack;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i])
        return (/*unconst*/ wchar_t *)haystack;
      if (!haystack[i])
        break;
      if (needle[i] != haystack[i])
        break;
    }
    if (!*haystack++)
      break;
  }
  return 0;
}

TEST(wcsstr, special) {
  MAKESTRING(haystack, L"abc123def");
  ASSERT_STREQ(&haystack[0], wcsstr(haystack, haystack));
  ASSERT_STREQ(&haystack[0], wcsstr(haystack, L""));
  free(haystack);
}

TEST(wcsstr, test_emptyString_isFoundAtBeginning) {
  MAKESTRING(haystack, L"abc123def");
  ASSERT_STREQ(&haystack[0], wcsstr(haystack, gc(wcsdup(L""))));
  free(haystack);
}

TEST(wcsstr, test_notFound) {
  MAKESTRING(haystack, L"abc123def");
  ASSERT_EQ(NULL, wcsstr(haystack, gc(wcsdup(L"xyz"))));
  free(haystack);
}

TEST(wcsstr, test_notFound1) {
  MAKESTRING(haystack, L"abc123def");
  ASSERT_EQ(NULL, wcsstr(haystack, gc(wcsdup(L"x"))));
  free(haystack);
}

TEST(wcsstr, test_middleOfString) {
  alignas(16) wchar_t hog[] = L"abc123def";
  MAKESTRING(haystack, hog);
  ASSERT_STREQ(&haystack[3], wcsstr(haystack, gc(wcsdup(L"123"))));
  free(haystack);
}

TEST(wcsstr, test_endOfString) {
  MAKESTRING(haystack, L"abc123def");
  ASSERT_STREQ(&haystack[8], wcsstr(haystack, gc(wcsdup(L"f"))));
  free(haystack);
}

TEST(wcsstr, test_secondXmmWord) {
  MAKESTRING(haystack, L"eeeeeeeeeeeeeeeebbbbbbbbbbb123");
  ASSERT_STREQ(&haystack[27], wcsstr(haystack, gc(wcsdup(L"123"))));
  free(haystack);
}

TEST(wcsstr, test_overlapsXmmWords) {
  MAKESTRING(haystack, L"eeeeeeeeeeeeeeeebbbbbbbbbbbbbbb");
  ASSERT_STREQ(&haystack[15], wcsstr(haystack, gc(wcsdup(L"eb"))));
  free(haystack);
}

TEST(wcsstr, test) {
  ASSERT_EQ(NULL, wcsstr(L"x86_64-linux-musl-gcc", L"clang"));
  ASSERT_STREQ(L"gcc", wcsstr(L"x86_64-linux-musl-gcc", L"gcc"));
  ASSERT_EQ(NULL, wcsstr(L"-Wl,--gc-sections", L"stack-protector"));
  ASSERT_EQ(NULL, wcsstr(L"-Wl,--gc-sections", L"sanitize"));
  ASSERT_STREQ(L"x", wcsstr(L"x", L"x"));
}

TEST(wcsstr, safety) {
  int pagesz = sysconf(_SC_PAGESIZE);
  char *map = mmap(0, pagesz * 2, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  npassert(map != MAP_FAILED);
  npassert(!mprotect(map + pagesz, pagesz, PROT_NONE));
  for (int haylen = 1; haylen < 128; ++haylen) {
    wchar_t *hay = (wchar_t *)(map + pagesz) - (haylen + 1);
    for (int i = 0; i < haylen; ++i) {
      hay[i] = rand() & 0x03030303;
      if (!hay[i])
        hay[i] = 1;
    }
    hay[haylen] = 0;
    for (int neelen = 1; neelen < haylen; ++neelen) {
      wchar_t *nee = hay + (haylen + 1) - (neelen + 1);
      ASSERT_EQ(wcsstr_naive(hay, nee), wcsstr(hay, nee));
    }
  }
  munmap(map, pagesz * 2);
}

TEST(wcsstr, breakit) {
  wchar_t *p;
  p = gc(calloc(sizeof(wchar_t), 32));
  p[0] = 'c';
  p[1] = 'c';
  p[10] = 'b';
  ASSERT_EQ(NULL, wcsstr(p, L"b"));
}

/*
 *     memmem naive        l:    43,783c    14,142ns   m:    31,285c    10,105ns
 *     memmem              l:     2,597c       839ns   m:     2,612c       844ns
 *     memmem              l:       509c       164ns   m:       599c       193ns
 *
 *     wcsstr naive        l:   103,057c    33,287ns   m:    47,035c    15,192ns
 *     wcsstr              l:     3,186c     1,029ns   m:     3,218c     1,039ns
 *     wcsstr torture 1    l:        27c         9ns   m:        61c        20ns
 *     wcsstr torture 2    l:     2,322c       750ns   m:     2,362c       763ns
 *     wcsstr torture 4    l:     2,407c       777ns   m:     2,448c       791ns
 *     wcsstr torture 8    l:     2,803c       905ns   m:     2,862c       924ns
 *     wcsstr torture 16   l:     4,559c     1,473ns   m:     3,614c     1,167ns
 *     wcsstr torture 32   l:     5,324c     1,720ns   m:     5,577c     1,801ns
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
BENCH(wcsstr, bench) {
  wchar_t *hyperion = gc(utf8to32(kHyperion, -1, 0));
  EZBENCH2("wcsstr naive", donothing,
           __expropriate(wcsstr_naive(hyperion, L"THE END")));
  EZBENCH2("wcsstr", donothing, __expropriate(wcsstr(hyperion, L"THE END")));
  EZBENCH2(
      "wcsstr torture 1", donothing,
      __expropriate(wcsstr(
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
          L"b")));
  EZBENCH2(
      "wcsstr torture 2", donothing,
      __expropriate(wcsstr(
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
          L"ab")));
  EZBENCH2(
      "wcsstr torture 4", donothing,
      __expropriate(wcsstr(
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
          L"aaab")));
  EZBENCH2(
      "wcsstr torture 8", donothing,
      __expropriate(wcsstr(
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
          L"aaaaaaab")));
  EZBENCH2(
      "wcsstr torture 16", donothing,
      __expropriate(wcsstr(
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
          L"aaaaaaaaaaaaaaab")));
  EZBENCH2(
      "wcsstr torture 32", donothing,
      __expropriate(wcsstr(
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
          L"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab")));
}
