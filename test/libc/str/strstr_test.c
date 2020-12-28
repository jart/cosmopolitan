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
#include "libc/runtime/gc.h"
#include "libc/str/internal.h"
#include "libc/testlib/testlib.h"

#define MAKESTRING(NAME, VALUE) \
  char *NAME = strcpy(tmalloc(sizeof(VALUE) + 16), VALUE)

char *strstr$kmp(const char *haystak, const char *needle) {
  return memmem(haystak, strlen(haystak), needle, strlen(needle));
}

char *(*strstri)(const char *, const char *) = strstr$kmp;

FIXTURE(strstr, sse42) {
  if (X86_HAVE(SSE4_2)) {
    strstri = strstr$sse42;
  }
}

TEST(strstr, test_emptyString_isFoundAtBeginning) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[0], strstri(haystack, gc(strdup(""))));
  ASSERT_STREQ(&haystack[0], strstr(haystack, gc(strdup(""))));
  tfree(haystack);
}

TEST(strstr, test_notFound) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_EQ(NULL, strstri(haystack, gc(strdup("xyz"))));
  ASSERT_EQ(NULL, strstr(haystack, gc(strdup("xyz"))));
  tfree(haystack);
}

TEST(strstr, test_middleOfString) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[3], strstri(haystack, gc(strdup("123"))));
  ASSERT_STREQ(&haystack[3], strstr(haystack, gc(strdup("123"))));
  tfree(haystack);
}

TEST(strstr, test_endOfString) {
  MAKESTRING(haystack, "abc123def");
  ASSERT_STREQ(&haystack[8], strstri(haystack, gc(strdup("f"))));
  ASSERT_STREQ(&haystack[8], strstr(haystack, gc(strdup("f"))));
  tfree(haystack);
}

TEST(strstr, test_secondXmmWord) {
  MAKESTRING(haystack, "eeeeeeeeeeeeeeeebbbbbbbbbbb123");
  ASSERT_STREQ(&haystack[27], strstri(haystack, gc(strdup("123"))));
  ASSERT_STREQ(&haystack[27], strstr(haystack, gc(strdup("123"))));
  tfree(haystack);
}

TEST(strstr, test_overlapsXmmWords) {
  MAKESTRING(haystack, "eeeeeeeeeeeeeeeebbbbbbbbbbbbbbb");
  ASSERT_STREQ(&haystack[15], strstri(haystack, gc(strdup("eb"))));
  ASSERT_STREQ(&haystack[15], strstr(haystack, gc(strdup("eb"))));
  tfree(haystack);
}
