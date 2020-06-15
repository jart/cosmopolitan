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
