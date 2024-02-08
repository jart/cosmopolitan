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
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/cachesize.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

int (*memcmpi)(const void *, const void *, size_t) = memcmp;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § emptiness                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strcmp, emptyString) {
  EXPECT_EQ(0, strcmp("", ""));
  EXPECT_NE(0, strcmp("", "a"));
}

TEST(strcasecmp, emptyString) {
  EXPECT_EQ(0, strcasecmp("", ""));
  EXPECT_NE(0, strcasecmp("", "a"));
}

TEST(strcmp16, emptyString) {
  EXPECT_EQ(0, strcmp16(u"", u""));
  EXPECT_NE(0, strcmp16(u"", u"a"));
}

TEST(strcasecmp16, emptyString) {
  EXPECT_EQ(0, strcasecmp16(u"", u""));
  EXPECT_NE(0, strcasecmp16(u"", u"a"));
}

TEST(wcscmp, emptyString) {
  EXPECT_EQ(0, wcscmp(L"", L""));
  EXPECT_NE(0, wcscmp(L"", L"a"));
}

TEST(wcscasecmp, emptyString) {
  EXPECT_EQ(0, wcscasecmp(L"", L""));
  EXPECT_NE(0, wcscasecmp(L"", L"a"));
}

TEST(strncmp, nullString) {
  char *s1 = malloc(0);
  char *s2 = malloc(0);
  ASSERT_NE(s1, s2);
  ASSERT_EQ(0, strncmp(s1, s2, 0));
  free(s2);
  free(s1);
}

TEST(strncmp, emptyString) {
  char *s1 = strcpy(malloc(1), "");
  char *s2 = strcpy(malloc(1), "");
  ASSERT_EQ(0, strncmp(s1, s2, 0));
  ASSERT_EQ(0, strncmp(s1, s2, 1));
  ASSERT_EQ(0, strncmp(s1, s2, -1));
  ASSERT_EQ(0, strncmp(s1, s1, -1));
  ASSERT_EQ(0, strncmp(s2, s2, -1));
  free(s2);
  free(s1);
}

TEST(strncasecmp, emptyString) {
  char *s1 = strcpy(malloc(1), "");
  char *s2 = strcpy(malloc(1), "");
  ASSERT_EQ(0, strncasecmp(s1, s2, 0));
  ASSERT_EQ(0, strncasecmp(s1, s2, 1));
  ASSERT_EQ(0, strncasecmp(s1, s2, -1));
  ASSERT_EQ(0, strncasecmp(s1, s1, -1));
  ASSERT_EQ(0, strncasecmp(s2, s2, -1));
  free(s2);
  free(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § inequality                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testInequality) {
  char *s1 = strcpy(malloc(2), "1");
  char *s2 = strcpy(malloc(1), "");
  ASSERT_EQ(0, strncmp(s1, s2, 0));
  ASSERT_GT(strncmp(s1, s2, 1), 0);
  ASSERT_LT(strncmp(s2, s1, 1), 0);
  free(s2);
  free(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § does it work?                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(memcmp, test) {
  EXPECT_EQ(memcmpi("\200", "\200", 1), 0);
  EXPECT_LT(memcmpi("\177", "\200", 1), 0);
  EXPECT_GT(memcmpi("\200", "\177", 1), 0);
  EXPECT_EQ(memcmpi("", "", 0), 0);
  EXPECT_EQ(memcmpi("a", "a", 1), 0);
  EXPECT_GT(memcmpi("a", "A", 1), 0);
  EXPECT_LT(memcmpi("A", "a", 1), 0);
  EXPECT_LT(memcmpi("\001", "\377", 1), 0);
  EXPECT_GT(memcmpi("\377", "\001", 1), 0);
  EXPECT_EQ(memcmpi("a", "aa", 1), 0);
  EXPECT_EQ(memcmpi("aa", "a", 1), 0);
  EXPECT_LT(memcmpi("a", "aa", 2), 0);
  EXPECT_GT(memcmpi("aa", "a", 2), 0);
  EXPECT_LT(memcmpi("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377", 16), 0);
  EXPECT_GT(memcmpi("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001", 16), 0);
  EXPECT_LT(memcmpi("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377", 17), 0);
  EXPECT_GT(memcmpi("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001", 17), 0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377", 32),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001", 32),
            0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377", 33),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001", 33),
            0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377", 34),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001", 34),
            0);
}

TEST(strcmp, testItWorks) {
  EXPECT_EQ(strcmp("", ""), 0);
  EXPECT_EQ(strcmp("a", "a"), 0);
  EXPECT_GT(strcmp("a", "A"), 0);
  EXPECT_LT(strcmp("A", "a"), 0);
  EXPECT_LT(strcmp("\001", "\377"), 0);
  EXPECT_GT(strcmp("\377", "\001"), 0);
  EXPECT_LT(strcmp("a", "aa"), 0);
  EXPECT_GT(strcmp("aa", "a"), 0);
  EXPECT_LT(strcmp("a\000", "aa\000"), 0);
  EXPECT_GT(strcmp("aa\000", "a\000"), 0);
  EXPECT_LT(strcmp("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmp("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmp("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmp("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                   "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(strcasecmp, testItWorks) {
  EXPECT_EQ(strcasecmp("", ""), 0);
  EXPECT_EQ(strcasecmp("a", "a"), 0);
  EXPECT_EQ(strcasecmp("a", "A"), 0);
  EXPECT_EQ(strcasecmp("A", "a"), 0);
  EXPECT_LT(strcasecmp("a", "z"), 0);
  EXPECT_GT(strcasecmp("z", "a"), 0);
  EXPECT_LT(strcasecmp("\001", "\377"), 0);
  EXPECT_GT(strcasecmp("\377", "\001"), 0);
  EXPECT_LT(strcasecmp("a", "aa"), 0);
  EXPECT_GT(strcasecmp("aa", "a"), 0);
  EXPECT_LT(strcasecmp("a\000", "aa\000"), 0);
  EXPECT_GT(strcasecmp("aa\000", "a\000"), 0);
  EXPECT_LT(strcasecmp("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcasecmp("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcasecmp("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcasecmp("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcasecmp("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                       "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(strcmp16, testItWorks) {
  EXPECT_EQ(strcmp16(u"", u""), 0);
  EXPECT_EQ(strcmp16(u"a", u"a"), 0);
  EXPECT_GT(strcmp16(u"a", u"A"), 0);
  EXPECT_LT(strcmp16(u"A", u"a"), 0);
  EXPECT_LT(strcmp16(u"\001", u"\377"), 0);
  EXPECT_GT(strcmp16(u"\377", u"\001"), 0);
  EXPECT_LT(strcmp16(u"a", u"aa"), 0);
  EXPECT_GT(strcmp16(u"aa", u"a"), 0);
  EXPECT_LT(strcmp16(u"a\000", u"aa\000"), 0);
  EXPECT_GT(strcmp16(u"aa\000", u"a\000"), 0);
  EXPECT_LT(strcmp16(u"aaaaaaaaaaaaaaa\001", u"aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmp16(u"aaaaaaaaaaaaaaa\377", u"aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmp16(u"aaaaaaaaaaaaaaaa\001", u"aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmp16(u"aaaaaaaaaaaaaaaa\377", u"aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmp16(u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                     u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp16(u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                     u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmp16(u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                     u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp16(u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                     u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmp16(u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                     u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmp16(u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                     u"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(wcscmp, testItWorks) {
  EXPECT_EQ(wcscmp(L"", L""), 0);
  EXPECT_EQ(wcscmp(L"a", L"a"), 0);
  EXPECT_GT(wcscmp(L"a", L"A"), 0);
  EXPECT_LT(wcscmp(L"A", L"a"), 0);
  EXPECT_LT(wcscmp(L"\001", L"\377"), 0);
  EXPECT_GT(wcscmp(L"\377", L"\001"), 0);
  EXPECT_LT(wcscmp(L"a", L"aa"), 0);
  EXPECT_GT(wcscmp(L"aa", L"a"), 0);
  EXPECT_LT(wcscmp(L"a", L"aa"), 0);
  EXPECT_GT(wcscmp(L"aa", L"a"), 0);
  EXPECT_LT(wcscmp(L"aaaaaaaaaaaaaaa\001", L"aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(wcscmp(L"aaaaaaaaaaaaaaa\377", L"aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(wcscmp(L"aaaaaaaaaaaaaaaa\001", L"aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(wcscmp(L"aaaaaaaaaaaaaaaa\377", L"aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(wcscmp(L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
                   L"aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
}

TEST(strcasecmp, testItWorksCase) {
  EXPECT_EQ(0, strcasecmp("hello", "HELLO"));
  EXPECT_EQ(0, strcasecmp("hello", "Hello"));
  EXPECT_EQ(0, strcasecmp("hello", "hello"));
  EXPECT_NE(0, strcasecmp("hello", "yello"));
}

TEST(strcasecmp16, testItWorksCase) {
  EXPECT_EQ(0, strcasecmp16(u"hello", u"HELLO"));
  EXPECT_EQ(0, strcasecmp16(u"hello", u"Hello"));
  EXPECT_EQ(0, strcasecmp16(u"hello", u"hello"));
  EXPECT_NE(0, strcasecmp16(u"hello", u"yello"));
}

TEST(wcscasecmp, testItWorksCase) {
  EXPECT_EQ(0, wcscasecmp(L"hello", L"HELLO"));
  EXPECT_EQ(0, wcscasecmp(L"hello", L"Hello"));
  EXPECT_EQ(0, wcscasecmp(L"hello", L"hello"));
  EXPECT_NE(0, wcscasecmp(L"hello", L"yello"));
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § nontrivial length                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testEqualManyNs) {
  char *s1 = malloc(4096);
  char *s2 = malloc(4096);
  memset(s1, 7, 4096);
  memset(s2, 7, 4096);
  s1[4096 - 1] = '\0';
  s2[4096 - 1] = '\0';
  for (unsigned i = 1; i <= 128; ++i) {
    ASSERT_EQ(0, strncmp(s1 + 4096 - i, s2 + 4096 - i, i + 0));
    ASSERT_EQ(0, strncmp(s1 + 4096 - i, s2 + 4096 - i, i + 1));
  }
  free(s2);
  free(s1);
}

TEST(strncmp, testNotEqualManyNs) {
  char *s1 = malloc(4096);
  char *s2 = malloc(4096);
  for (unsigned i = 1; i <= 128; ++i) {
    memset(s1, 7, 4096);
    memset(s2, 7, 4096);
    s1[4096 - 1] = (unsigned char)0;
    s2[4096 - 1] = (unsigned char)255;
    ASSERT_EQ(-255, strncmp(s1 + 4096 - i, s2 + 4096 - i, i + 0));
    ASSERT_EQ(-255, strncmp(s1 + 4096 - i, s2 + 4096 - i, i + 1));
  }
  free(s2);
  free(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § nul termination vs. explicit length        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testStringNulTerminatesBeforeExplicitLength) {
  const char kRdi[] = "";
  const char kRsi[] = "TZ=America/Los_Angeles";
  char *rdi = memcpy(malloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char *rsi = memcpy(malloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncmp(rdi, rdi, rdx), 0);
  EXPECT_LT(strncmp(rdi, rsi, rdx), 0);
  EXPECT_GT(strncmp(rsi, rdi, rdx), 0);
  free(rsi);
  free(rdi);
}

TEST(strncasecmp, testStringNulTerminatesBeforeExplicitLength) {
  const char kRdi[] = "";
  const char kRsi[] = "TZ=America/Los_Angeles";
  char *rdi = memcpy(malloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char *rsi = memcpy(malloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncasecmp(rdi, rdi, rdx), 0);
  EXPECT_LT(strncasecmp(rdi, rsi, rdx), 0);
  EXPECT_GT(strncasecmp(rsi, rdi, rdx), 0);
  free(rsi);
  free(rdi);
}

TEST(strncmp16, testStringNulTerminatesBeforeExplicitLength) {
  const char16_t kRdi[] = u"";
  const char16_t kRsi[] = u"TZ=America/Los_Angeles";
  char16_t *rdi = memcpy(malloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char16_t *rsi = memcpy(malloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncmp16(rdi, rdi, rdx), 0);
  EXPECT_LT(strncmp16(rdi, rsi, rdx), 0);
  EXPECT_GT(strncmp16(rsi, rdi, rdx), 0);
  free(rsi);
  free(rdi);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § two's complement bane                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strcmp, testTwosComplementBane_hasUnsignedBehavior) {
  EXPECT_EQ(strcmp("\200", "\200"), 0);
  EXPECT_LT(strcmp("\x7f", "\x80"), 0);
  EXPECT_GT(strcmp("\x80", "\x7f"), 0);
}

TEST(strcasecmp, testTwosComplementBane_hasUnsignedBehavior) {
  EXPECT_EQ(strcasecmp("\200", "\200"), 0);
  EXPECT_LT(strcasecmp("\x7f", "\x80"), 0);
  EXPECT_GT(strcasecmp("\x80", "\x7f"), 0);
}

TEST(memcmp, testTwosComplementBane_unsignedBehavior) {
  EXPECT_EQ(memcmpi("\200", "\200", 1), 0);
  EXPECT_LT(memcmpi("\177", "\200", 1), 0);
  EXPECT_GT(memcmpi("\200", "\177", 1), 0);
  EXPECT_EQ(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200", 34),
            0);
  EXPECT_LT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\177",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200", 34),
            0);
  EXPECT_GT(memcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\200",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\177", 34),
            0);
}

TEST(strcmp16, testTwosComplementBane_hasUnsignedBehavior) {
  char16_t *B1 = malloc(8);
  char16_t *B2 = malloc(8);
  B1[1] = L'\0';
  B2[1] = L'\0';
  EXPECT_EQ(strcmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\x00\x80", 2)), 0);
  EXPECT_LT(strcmp16(memcpy(B1, "\xff\x7f", 2), memcpy(B2, "\x00\x80", 2)), 0);
  EXPECT_GT(strcmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\xff\x7f", 2)), 0);
  free(B2);
  free(B1);
}

TEST(strncmp16, testTwosComplementBane_hasUnsignedBehavior) {
  char16_t *B1 = malloc(4);
  char16_t *B2 = malloc(4);
  EXPECT_EQ(strncmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\x00\x80", 2), 1),
            0);
  EXPECT_LT(strncmp16(memcpy(B1, "\xff\x7f", 2), memcpy(B2, "\x00\x80", 2), 1),
            0);
  EXPECT_GT(strncmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\xff\x7f", 2), 1),
            0);
  free(B2);
  free(B1);
}

TEST(wcscmp, testTwosComplementBane) {
  wchar_t *B1 = malloc(8);
  wchar_t *B2 = malloc(8);
  B1[1] = L'\0';
  B2[1] = L'\0';
  EXPECT_EQ(wcscmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                   memcpy(B2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_LT(0, wcscmp(memcpy(B1, "\xff\xff\xff\x7f", 4),
                      memcpy(B2, "\x00\x00\x00\x80", 4)));
  EXPECT_LT(wcscmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                   memcpy(B2, "\xff\xff\xff\x7f", 4)),
            0);
  free(B2);
  free(B1);
}

TEST(wcsncmp, testTwosComplementBane) {
  wchar_t *B1 = malloc(4);
  wchar_t *B2 = malloc(4);
  EXPECT_EQ(wcsncmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                    memcpy(B2, "\x00\x00\x00\x80", 4), 1),
            0);
  EXPECT_GT(wcsncmp(memcpy(B1, "\xff\xff\xff\x7f", 4),
                    memcpy(B2, "\x00\x00\x00\x80", 4), 1),
            0);
  EXPECT_LT(wcsncmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                    memcpy(B2, "\xff\xff\xff\x7f", 4), 1),
            0);
  free(B2);
  free(B1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § benchmarks                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

dontinline int strcmp_pure(const char *a, const char *b) {
  for (; *a == *b; a++, b++) {
    if (!*a) break;
  }
  return (*a & 0xff) - (*b & 0xff);
}

dontinline int strcasecmp_pure(const char *a, const char *b) {
  for (; *a && *b; a++, b++) {
    if (!(*a == *b || tolower(*a & 0xff) == tolower(*b & 0xff))) {
      break;
    }
  }
  return tolower(*a & 0xff) - tolower(*b & 0xff);
}

char *randomize_buf2str(size_t size, char *data) {
  assert(data);
  rngset(data, size, _rand64, -1);
  data[size - 1] = '\0';
  return data;
}

char *longstringislong(size_t size, char *data) {
  unsigned i;
  assert(data);
  randomize_buf2str(size, data);
  for (i = 0; i < size; ++i) {
    data[i] |= 1u << (i & 5);
  }
  data[size - 1] = '\0';
  return data;
}

void randomize_buf2str_dupe(size_t size, char data[size], char dupe[size]) {
  randomize_buf2str(size, data);
  memcpy(dupe, data, size);
}

void longstringislong_dupe(size_t size, char data[size], char dupe[size]) {
  longstringislong(size, data);
  memcpy(dupe, data, size);
}

BENCH(bench_00_strcmp, bench) {
  size_t size;
  char *dupe, *data;

  size = 14139;
  data = gc(malloc(size));
  dupe = gc(malloc(size));

  EZBENCH2("strcmp [identity]", longstringislong(size, data),
           __expropriate(strcmp(__veil("r", data), data)));

  EZBENCH2("strcmp [2 diff]", donothing,
           __expropriate(strcmp(__veil("r", "hi"), __veil("r", "there"))));
  EZBENCH2("scmppure [2 diff]", donothing,
           __expropriate(strcmp_pure(__veil("r", "hi"), __veil("r", "there"))));

  EZBENCH2("strcmp [2 dupe]", randomize_buf2str_dupe(2, data, dupe),
           __expropriate(strcmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2("scmp_pure [2 dupe]", randomize_buf2str_dupe(2, data, dupe),
           __expropriate(strcmp_pure(__veil("r", data), __veil("r", dupe))));

  EZBENCH2("strcmp [4 dupe]", randomize_buf2str_dupe(4, data, dupe),
           __expropriate(strcmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2("scmp_pure [4 dupe]", randomize_buf2str_dupe(4, data, dupe),
           __expropriate(strcmp_pure(__veil("r", data), __veil("r", dupe))));

  EZBENCH2("strcmp [8 dupe]", randomize_buf2str_dupe(8, data, dupe),
           __expropriate(strcmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2("scmp_pure [8 dupe]", randomize_buf2str_dupe(8, data, dupe),
           __expropriate(strcmp_pure(__veil("r", data), __veil("r", dupe))));

  EZBENCH2("strcmp [sdupe]", randomize_buf2str_dupe(size, data, dupe),
           __expropriate(strcmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2("scmp_pure [sdupe]", randomize_buf2str_dupe(size, data, dupe),
           __expropriate(strcmp_pure(__veil("r", data), __veil("r", dupe))));

  EZBENCH2("strcmp [ldupe]", longstringislong_dupe(size, data, dupe),
           __expropriate(strcmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2("scmp_pure [ldupe]", longstringislong_dupe(size, data, dupe),
           __expropriate(strcmp_pure(__veil("r", data), __veil("r", dupe))));
}

BENCH(bench_01_strcasecmp, bench) {
  size_t size;
  char *dupe, *data;
  size = 141393;
  data = gc(malloc(size));
  dupe = gc(malloc(size));

  EZBENCH2("strcasecmp [iden]", longstringislong(size, data),
           __expropriate(strcasecmp(__veil("r", data), data)));

  EZBENCH2("strcasecmp [sdupe]", randomize_buf2str_dupe(size, data, dupe),
           __expropriate(strcasecmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2(
      "sccmp_pure [sdupe]", randomize_buf2str_dupe(size, data, dupe),
      __expropriate(strcasecmp_pure(__veil("r", data), __veil("r", dupe))));

  EZBENCH2("strcasecmp [ldupe]", longstringislong_dupe(size, data, dupe),
           __expropriate(strcasecmp(__veil("r", data), __veil("r", dupe))));
  EZBENCH2(
      "sccmp_pure [ldupe]", longstringislong_dupe(size, data, dupe),
      __expropriate(strcasecmp_pure(__veil("r", data), __veil("r", dupe))));
}

BENCH(memcmp, bench) {
  volatile char *copy = gc(strdup(kHyperion));
  EZBENCH2("memcmp big", donothing,
           __expropriate(memcmp(kHyperion, (void *)copy, kHyperionSize)));
  copy = gc(strdup("tough little ship"));
  EZBENCH2("memcmp 18", donothing,
           __expropriate(memcmp("tough little ship", (void *)copy, 18)));
}

/* jart
 *     strcmp [identity]   l:         3c         1ns   m:        30c        10ns
 *     strcmp [2 diff]     l:         4c         1ns   m:        30c        10ns
 *     scmppure [2 diff]   l:         3c         1ns   m:        31c        10ns
 *     strcmp [2 dupe]     l:         8c         3ns   m:        39c        13ns
 *     scmp_pure [2 dupe]  l:         6c         2ns   m:        34c        11ns
 *     strcmp [4 dupe]     l:         9c         3ns   m:        40c        13ns
 *     scmp_pure [4 dupe]  l:         9c         3ns   m:        38c        12ns
 *     strcmp [8 dupe]     l:        10c         3ns   m:        40c        13ns
 *     scmp_pure [8 dupe]  l:        11c         4ns   m:        41c        13ns
 *     strcmp [sdupe]      l:        87c        28ns   m:       121c        39ns
 *     scmp_pure [sdupe]   l:       188c        61ns   m:       294c        95ns
 *     strcmp [ldupe]      l:     3,458c     1,117ns   m:     3,486c     1,126ns
 *     scmp_pure [ldupe]   l:    11,441c     3,695ns   m:    11,520c     3,721ns
 *     strcasecmp [iden]   l:         3c         1ns   m:        30c        10ns
 *     strcasecmp [sdupe]  l:       105c        34ns   m:       156c        50ns
 *     sccmp_pure [sdupe]  l:       644c       208ns   m:       963c       311ns
 *     strcasecmp [ldupe]  l:    36,527c    11,798ns   m:    36,582c    11,816ns
 *     sccmp_pure [ldupe]  l:   365,880c   118,177ns   m:   365,721c   118,125ns
 *     memcmp big          l:     2,050c       662ns   m:     2,093c       676ns
 *     memcmp 18           l:         6c         2ns   m:        35c        11ns
 */

/* jart+intel
 *     strcmp [identity]   l:         1c         0ns   m:        28c         9ns
 *     strcmp [2 diff]     l:         2c         1ns   m:        29c         9ns
 *     scmppure [2 diff]   l:         2c         1ns   m:        29c         9ns
 *     strcmp [2 dupe]     l:         8c         3ns   m:        40c        13ns
 *     scmp_pure [2 dupe]  l:         5c         2ns   m:        32c        10ns
 *     strcmp [4 dupe]     l:         9c         3ns   m:        41c        13ns
 *     scmp_pure [4 dupe]  l:         7c         2ns   m:        34c        11ns
 *     strcmp [8 dupe]     l:        10c         3ns   m:        40c        13ns
 *     scmp_pure [8 dupe]  l:        10c         3ns   m:        39c        13ns
 *     strcmp [sdupe]      l:        57c        18ns   m:        87c        28ns
 *     scmp_pure [sdupe]   l:       191c        62ns   m:       224c        72ns
 *     strcmp [ldupe]      l:     1,667c       538ns   m:     1,708c       552ns
 *     scmp_pure [ldupe]   l:    10,988c     3,549ns   m:    11,055c     3,571ns
 *     strcasecmp [iden]   l:         2c         1ns   m:        31c        10ns
 *     strcasecmp [sdupe]  l:       121c        39ns   m:       160c        52ns
 *     sccmp_pure [sdupe]  l:       684c       221ns   m:       702c       227ns
 *     strcasecmp [ldupe]  l:    34,325c    11,087ns   m:    35,954c    11,613ns
 *     sccmp_pure [ldupe]  l:   361,449c   116,746ns   m:   366,022c   118,223ns
 *     memcmp big          l:     2,040c       659ns   m:     2,083c       673ns
 *     memcmp 18           l:         5c         2ns   m:        35c        11ns
 */
