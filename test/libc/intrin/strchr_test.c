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
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(strchr, blank) {
  const char *const blank = "";
  EXPECT_EQ(NULL, strchr(blank, '#'));
  EXPECT_EQ(blank, strchr(blank, '\0'));
}

TEST(strchr, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", strchr(buf, 't'));
}
TEST(strchr, testsse) {
  char buf[] = "hellohellohellohellohellohellohellohello"
               "theretheretheretheretheretheretherethere";
  EXPECT_STREQ("theretheretheretheretheretheretherethere", strchr(buf, 't'));
}
TEST(rawmemchr, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", rawmemchr(buf, 't'));
}
TEST(strchrnul, text) {
  char buf[] = "hellothere";
  EXPECT_STREQ("there", strchrnul(buf, 't'));
}

TEST(strchr, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", strchr(buf, '\0'));
}
TEST(rawmemchr, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", rawmemchr(buf, '\0'));
}
TEST(strchrnul, nulTerminator) {
  char buf[] = "hellothere";
  EXPECT_STREQ("", strchrnul(buf, '\0'));
}

TEST(strchr, notFound_returnsNul) {
  char buf[] = "hellothere";
  EXPECT_EQ(NULL, strchr(buf, 'z'));
}
TEST(strchrnul, notFound_returnsPointerToNulByte) {
  char buf[] = "hi";
  EXPECT_STREQ("", strchrnul(buf, 'z'));
  EXPECT_EQ(&buf[2], strchrnul(buf, 'z'));
}

char *strchr_pure(const char *s, int c) {
  for (c &= 0xff;; ++s) {
    if ((*s & 0xff) == c) return (char *)s;
    if (!*s) return NULL;
  }
}

TEST(strchr, fuzz) {
  char *p;
  int i, j;
  p = calloc(1, 64);
  for (i = -2; i < 257; ++i) {
    for (j = 0; j < 17; ++j) {
      rngset(p, 63, rdseed, -1);
      ASSERT_EQ(strchr_pure(p + j, i), strchr(p + j, i));
    }
  }
  free(p);
}

BENCH(strchr, bench) {
  EZBENCH2("strchr 0", donothing, __expropriate(strchr(__veil("r", ""), 0)));
  EZBENCH2("strchr 5", donothing,
           __expropriate(strchr(__veil("r", "hello"), 'o')));
  EZBENCH2("strchr 8", donothing,
           __expropriate(strchr(__veil("r", "hellzzzo"), 'o')));
  EZBENCH2("strchr 17", donothing,
           __expropriate(strchr(__veil("r", "hellzzzhellzzzeeo"), 'o')));
  EZBENCH2("strchr 34", donothing,
           __expropriate(
               strchr(__veil("r", "hellzzzhellzzzeeAhellzzzhellzzzeeo"), 'o')));
}

char *memchr_pure(const char *m, int c, size_t n) {
  const unsigned char *p, *pe;
  for (c &= 0xff, p = (const unsigned char *)m, pe = p + n; p < pe; ++p) {
    if (*p == c) return (void *)p;
  }
  return NULL;
}

TEST(memchr, fuzz) {
  char *p;
  int i, j;
  p = malloc(64);
  for (i = -2; i < 257; ++i) {
    for (j = 0; j < 17; ++j) {
      rngset(p, 64, _rand64, -1);
      ASSERT_EQ(memchr(p + j, i, 64 - j), memchr_pure(p + j, i, 64 - j));
    }
  }
  free(p);
}

char *strchrnul_pure(const char *s, int c) {
  for (c &= 0xff;; ++s) {
    if ((*s & 0xff) == c) return (char *)s;
    if (!*s) return (void *)s;
  }
}

TEST(strchrnul, fuzz) {
  char *p;
  int i, j;
  p = calloc(1, 64);
  for (i = -2; i < 257; ++i) {
    for (j = 0; j < 17; ++j) {
      rngset(p, 63, _rand64, -1);
      ASSERT_EQ(strchrnul(p + j, i), strchrnul_pure(p + j, i));
    }
  }
  free(p);
}

void *rawmemchr_pure(const void *m, int c) {
  const unsigned char *s;
  for (c &= 255, s = m;; ++s) {
    if (*s == c) return (void *)s;
  }
}

TEST(rawmemchr, fuzz) {
  char *p;
  int i, j;
  p = malloc(64);
  for (i = -2; i < 257; ++i) {
    for (j = 0; j < 17; ++j) {
      rngset(p, 63, _rand64, -1);
      p[63] = i;
      ASSERT_EQ(rawmemchr(p + j, i), rawmemchr_pure(p + j, i));
    }
  }
  free(p);
}

/*
 *     strchr 0            l:        10c         3ns   m:        38c        12ns
 *     strchr 5            l:        13c         4ns   m:        42c        14ns
 *     strchr 8            l:        14c         5ns   m:        44c        14ns
 *     strchr 17           l:        13c         4ns   m:        45c        15ns
 *     strchr 34           l:        16c         5ns   m:        48c        16ns
 *     strchr z            l:       369c       119ns   m:       408c       132ns
 *     rawmemchr z         l:       236c        76ns   m:       391c       126ns
 *     memchr z            l:       357c       115ns   m:       423c       137ns
 *     strchr Z            l:     1,872c       605ns   m:     1,911c       617ns
 *     rawmemchr \0        l:     1,184c       382ns   m:     1,880c       607ns
 *     strlen              l:     1,174c       379ns   m:     1,237c       400ns
 *     memchr Z            l:     1,866c       603ns   m:     1,945c       628ns
 *     strchrnul z         l:       365c       118ns   m:       408c       132ns
 *     strchrnul Z         l:     1,871c       604ns   m:     1,914c       618ns
 */
BENCH(strchr, bench2) {
  char *strchr_(const char *, int) asm("strchr");
  char *strchrnul_(const char *, int) asm("strchrnul");
  char *memchr_(const char *, int, size_t) asm("memchr");
  char *strlen_(const char *) asm("strlen");
  char *rawmemchr_(const char *, int) asm("rawmemchr");
  EZBENCH2("strchr z", donothing, strchr_(kHyperion, 'z'));
  EZBENCH2("strchr Z", donothing, strchr_(kHyperion, 'Z'));
  EZBENCH2("memchr z", donothing, memchr_(kHyperion, 'z', kHyperionSize));
  EZBENCH2("memchr Z", donothing, memchr_(kHyperion, 'Z', kHyperionSize));
  EZBENCH2("rawmemchr z", donothing, rawmemchr_(kHyperion, 'z'));
  EZBENCH2("rawmemchr Z", donothing, rawmemchr_(kHyperion, 'z'));
  EZBENCH2("rawmemchr \\0", donothing, rawmemchr_(kHyperion, 0));
  EZBENCH2("strlen", donothing, strlen_(kHyperion));
  EZBENCH2("memchr Z", donothing, memchr_(kHyperion, 'Z', kHyperionSize));
  EZBENCH2("strchrnul z", donothing, strchrnul_(kHyperion, 'z'));
  EZBENCH2("strchrnul Z", donothing, strchrnul_(kHyperion, 'Z'));
}
