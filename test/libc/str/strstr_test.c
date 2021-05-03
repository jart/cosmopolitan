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
#include "libc/dce.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/gc.internal.h"
#include "libc/str/internal.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

#define MAKESTRING(NAME, VALUE) \
  char *NAME = strcpy(malloc(sizeof(VALUE) + 16), VALUE)

char *strstr_kmp(const char *haystak, const char *needle) {
  return memmem(haystak, strlen(haystak), needle, strlen(needle));
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

BENCH(strstr, bench) {
  EZBENCH2("strstr", donothing, EXPROPRIATE(strstr(kHyperion, "THE END")));
  EZBENCH2("strstr torture 1", donothing,
           EXPROPRIATE(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "b")));
  EZBENCH2("strstr torture 2", donothing,
           EXPROPRIATE(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "ab")));
  EZBENCH2("strstr torture 4", donothing,
           EXPROPRIATE(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaab")));
  EZBENCH2("strstr torture 8", donothing,
           EXPROPRIATE(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaab")));
  EZBENCH2("strstr torture 16", donothing,
           EXPROPRIATE(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaaaaaaaaaab")));
  EZBENCH2("strstr torture 32", donothing,
           EXPROPRIATE(strstr(
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab",
               "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab")));
}
