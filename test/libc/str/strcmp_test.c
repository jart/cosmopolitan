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
#include "libc/bits/bits.h"
#include "libc/dce.h"
#include "libc/macros.h"
#include "libc/nexgen32e/cachesize.h"
#include "libc/nexgen32e/tinystrcmp.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/rand/rand.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

int memcmp$sse2(const void *, const void *, size_t) hidden;
int memcmp$avx2(const void *, const void *, size_t) hidden;
int (*memcmpi)(const void *, const void *, size_t) = memcmp$sse2;

int strcmp$k8(const char *, const char *) hidden;
int strcmp$avx(const char *, const char *) hidden;
int (*strcmpi)(const char *, const char *) = tinystrcmp;

int strncmp$k8(const char *, const char *, size_t) hidden;
int strncmp$avx(const char *, const char *, size_t) hidden;
int (*strncmpi)(const char *, const char *, size_t) = tinystrncmp;

FIXTURE(strcmp, avx) {
  if (X86_HAVE(AVX)) {
    strcmpi = strcmp$avx;
    strncmpi = strncmp$avx;
  }
  if (X86_HAVE(AVX2)) {
    memcmpi = memcmp$avx2;
  }
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § emptiness                                  ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strcmp, emptyString) {
  EXPECT_EQ(0, strcmpi("", ""));
  EXPECT_NE(0, strcmpi("", "a"));
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

TEST(strncmp, emptyString) {
  char *s1 = strcpy(tmalloc(1), "");
  char *s2 = strcpy(tmalloc(1), "");
  ASSERT_EQ(0, strncmpi(s1, s2, 0));
  ASSERT_EQ(0, strncmpi(s1, s2, 1));
  ASSERT_EQ(0, strncmpi(s1, s2, -1));
  ASSERT_EQ(0, strncmpi(s1, s1, -1));
  ASSERT_EQ(0, strncmpi(s2, s2, -1));
  tfree(s2);
  tfree(s1);
}

TEST(strncasecmp, emptyString) {
  char *s1 = strcpy(tmalloc(1), "");
  char *s2 = strcpy(tmalloc(1), "");
  ASSERT_EQ(0, strncasecmp(s1, s2, 0));
  ASSERT_EQ(0, strncasecmp(s1, s2, 1));
  ASSERT_EQ(0, strncasecmp(s1, s2, -1));
  ASSERT_EQ(0, strncasecmp(s1, s1, -1));
  ASSERT_EQ(0, strncasecmp(s2, s2, -1));
  tfree(s2);
  tfree(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § inequality                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testInequality) {
  char *s1 = strcpy(tmalloc(2), "1");
  char *s2 = strcpy(tmalloc(1), "");
  ASSERT_EQ(0, strncmpi(s1, s2, 0));
  ASSERT_EQ('1', strncmpi(s1, s2, 1));
  ASSERT_EQ(-'1', strncmpi(s2, s1, 1));
  tfree(s2);
  tfree(s1);
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
  EXPECT_EQ(strcmpi("", ""), 0);
  EXPECT_EQ(strcmpi("a", "a"), 0);
  EXPECT_GT(strcmpi("a", "A"), 0);
  EXPECT_LT(strcmpi("A", "a"), 0);
  EXPECT_LT(strcmpi("\001", "\377"), 0);
  EXPECT_GT(strcmpi("\377", "\001"), 0);
  EXPECT_LT(strcmpi("a", "aa"), 0);
  EXPECT_GT(strcmpi("aa", "a"), 0);
  EXPECT_LT(strcmpi("a\000", "aa\000"), 0);
  EXPECT_GT(strcmpi("aa\000", "a\000"), 0);
  EXPECT_LT(strcmpi("aaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmpi("aaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmpi("aaaaaaaaaaaaaaaa\001", "aaaaaaaaaaaaaaaa\377"), 0);
  EXPECT_GT(strcmpi("aaaaaaaaaaaaaaaa\377", "aaaaaaaaaaaaaaaa\001"), 0);
  EXPECT_LT(strcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\377",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaa\001"),
            0);
  EXPECT_LT(strcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\001",
                    "aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377"),
            0);
  EXPECT_GT(strcmpi("aaaaaaaaabaaaaaaaaaaaaaaaaaaaaaaa\377",
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

TEST(strcasecmp8to16, testItWorksCase) {
  EXPECT_EQ(0, strcasecmp8to16("hello", u"HELLO"));
  EXPECT_EQ(0, strcasecmp8to16("hello", u"Hello"));
  EXPECT_EQ(0, strcasecmp8to16("hello", u"hello"));
  EXPECT_NE(0, strcasecmp8to16("hello", u"yello"));
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § nontrivial length                          ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testEqualManyNs) {
  char *s1 = tmalloc(PAGESIZE);
  char *s2 = tmalloc(PAGESIZE);
  memset(s1, 7, PAGESIZE);
  memset(s2, 7, PAGESIZE);
  s1[PAGESIZE - 1] = '\0';
  s2[PAGESIZE - 1] = '\0';
  for (unsigned i = 1; i <= 128; ++i) {
    ASSERT_EQ(0, strncmpi(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 0));
    ASSERT_EQ(0, strncmpi(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 1));
  }
  tfree(s2);
  tfree(s1);
}

TEST(strncmp, testNotEqualManyNs) {
  char *s1 = tmalloc(PAGESIZE);
  char *s2 = tmalloc(PAGESIZE);
  for (unsigned i = 1; i <= 128; ++i) {
    memset(s1, 7, PAGESIZE);
    memset(s2, 7, PAGESIZE);
    s1[PAGESIZE - 1] = (unsigned char)0;
    s2[PAGESIZE - 1] = (unsigned char)255;
    ASSERT_EQ(-255, strncmpi(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 0));
    ASSERT_EQ(-255, strncmpi(s1 + PAGESIZE - i, s2 + PAGESIZE - i, i + 1));
  }
  tfree(s2);
  tfree(s1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § nul termination vs. explicit length        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strncmp, testStringNulTerminatesBeforeExplicitLength) {
  const char kRdi[] = "";
  const char kRsi[] = "TZ=America/Los_Angeles";
  char *rdi = memcpy(tmalloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char *rsi = memcpy(tmalloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncmpi(rdi, rdi, rdx), 0);
  EXPECT_LT(strncmpi(rdi, rsi, rdx), 0);
  EXPECT_GT(strncmpi(rsi, rdi, rdx), 0);
  tfree(rsi);
  tfree(rdi);
}

TEST(strncasecmp, testStringNulTerminatesBeforeExplicitLength) {
  const char kRdi[] = "";
  const char kRsi[] = "TZ=America/Los_Angeles";
  char *rdi = memcpy(tmalloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char *rsi = memcpy(tmalloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncasecmp(rdi, rdi, rdx), 0);
  EXPECT_LT(strncasecmp(rdi, rsi, rdx), 0);
  EXPECT_GT(strncasecmp(rsi, rdi, rdx), 0);
  tfree(rsi);
  tfree(rdi);
}

TEST(strncmp16, testStringNulTerminatesBeforeExplicitLength) {
  const char16_t kRdi[] = u"";
  const char16_t kRsi[] = u"TZ=America/Los_Angeles";
  char16_t *rdi = memcpy(tmalloc(sizeof(kRdi)), kRdi, sizeof(kRdi));
  char16_t *rsi = memcpy(tmalloc(sizeof(kRsi)), kRsi, sizeof(kRsi));
  size_t rdx = 3;
  EXPECT_EQ(strncmp16(rdi, rdi, rdx), 0);
  EXPECT_LT(strncmp16(rdi, rsi, rdx), 0);
  EXPECT_GT(strncmp16(rsi, rdi, rdx), 0);
  tfree(rsi);
  tfree(rdi);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § two's complement bane                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

TEST(strcmp, testTwosComplementBane_hasUnsignedBehavior) {
  EXPECT_EQ(strcmpi("\200", "\200"), 0);
  EXPECT_LT(strcmpi("\x7f", "\x80"), 0);
  EXPECT_GT(strcmpi("\x80", "\x7f"), 0);
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
  char16_t *B1 = tmalloc(8);
  char16_t *B2 = tmalloc(8);
  B1[1] = L'\0';
  B2[1] = L'\0';
  EXPECT_EQ(strcmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\x00\x80", 2)), 0);
  EXPECT_LT(strcmp16(memcpy(B1, "\xff\x7f", 2), memcpy(B2, "\x00\x80", 2)), 0);
  EXPECT_GT(strcmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\xff\x7f", 2)), 0);
  tfree(B2);
  tfree(B1);
}

TEST(strncmp16, testTwosComplementBane_hasUnsignedBehavior) {
  char16_t *B1 = tmalloc(4);
  char16_t *B2 = tmalloc(4);
  EXPECT_EQ(strncmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\x00\x80", 2), 1),
            0);
  EXPECT_LT(strncmp16(memcpy(B1, "\xff\x7f", 2), memcpy(B2, "\x00\x80", 2), 1),
            0);
  EXPECT_GT(strncmp16(memcpy(B1, "\x00\x80", 2), memcpy(B2, "\xff\x7f", 2), 1),
            0);
  tfree(B2);
  tfree(B1);
}

TEST(wcscmp, testTwosComplementBane_hasSignedBehavior) {
  wchar_t *B1 = tmalloc(8);
  wchar_t *B2 = tmalloc(8);
  B1[1] = L'\0';
  B2[1] = L'\0';
  EXPECT_EQ(wcscmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                   memcpy(B2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_GT(wcscmp(memcpy(B1, "\xff\xff\xff\x7f", 4),
                   memcpy(B2, "\x00\x00\x00\x80", 4)),
            0);
  EXPECT_LT(wcscmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                   memcpy(B2, "\xff\xff\xff\x7f", 4)),
            0);
  tfree(B2);
  tfree(B1);
}

TEST(wcsncmp, testTwosComplementBane_hasSignedBehavior) {
  wchar_t *B1 = tmalloc(4);
  wchar_t *B2 = tmalloc(4);
  EXPECT_EQ(wcsncmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                    memcpy(B2, "\x00\x00\x00\x80", 4), 1),
            0);
  EXPECT_GT(wcsncmp(memcpy(B1, "\xff\xff\xff\x7f", 4),
                    memcpy(B2, "\x00\x00\x00\x80", 4), 1),
            0);
  EXPECT_LT(wcsncmp(memcpy(B1, "\x00\x00\x00\x80", 4),
                    memcpy(B2, "\xff\xff\xff\x7f", 4), 1),
            0);
  tfree(B2);
  tfree(B1);
}

/*───────────────────────────────────────────────────────────────────────────│─╗
│ test/libc/str/strcmp_test.c § benchmarks                                 ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

testonly noinline int strcmp$pure(const char *a, const char *b) {
  for (; *a == *b; a++, b++) {
    if (!*a) break;
  }
  return (*a & 0xff) - (*b & 0xff);
}

testonly noinline int strcasecmp$pure(const char *a, const char *b) {
  for (; *a && *b; a++, b++) {
    if (!(*a == *b || tolower(*a & 0xff) == tolower(*b & 0xff))) {
      break;
    }
  }
  return tolower(*a & 0xff) - tolower(*b & 0xff);
}

char *randomize_buf2str(size_t size, char data[size]) {
  rngset(data, size, rand64, -1);
  data[size - 1] = '\0';
  return data;
}

char *longstringislong(size_t size, char data[size]) {
  unsigned i;
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
  size = ROUNDDOWN(MAX(FRAMESIZE, getcachesize(kCpuCacheTypeData, 1)) / 2,
                   PAGESIZE);
  data = tgc(tmalloc(size));
  dupe = tgc(tmalloc(size));
  EZBENCH2("strcmp [identity]", longstringislong(size, data),
           EXPROPRIATE(strcmp(VEIL("r", data), data)));
  EZBENCH2("strcmp [short dupe]", randomize_buf2str_dupe(size, data, dupe),
           EXPROPRIATE(strcmp(VEIL("r", data), VEIL("r", dupe))));
  EZBENCH2("strcmp$pure [short dupe]", randomize_buf2str_dupe(size, data, dupe),
           EXPROPRIATE(strcmp$pure(VEIL("r", data), VEIL("r", dupe))));
  EZBENCH2("strcmp [long dupe]", longstringislong_dupe(size, data, dupe),
           EXPROPRIATE(strcmp(VEIL("r", data), VEIL("r", dupe))));
  EZBENCH2("strcmp$pure [long dupe]", longstringislong_dupe(size, data, dupe),
           EXPROPRIATE(strcmp$pure(VEIL("r", data), VEIL("r", dupe))));
}

BENCH(bench_01_strcasecmp, bench) {
  size_t size;
  char *dupe, *data;
  size = ROUNDDOWN(MAX(FRAMESIZE, getcachesize(kCpuCacheTypeData, 1)) / 2,
                   PAGESIZE);
  data = tgc(tmalloc(size));
  dupe = tgc(tmalloc(size));
  EZBENCH2("strcasecmp [identity]", longstringislong(size, data),
           EXPROPRIATE(strcasecmp(VEIL("r", data), data)));
  EZBENCH2("strcasecmp [short dupe]", randomize_buf2str_dupe(size, data, dupe),
           EXPROPRIATE(strcasecmp(VEIL("r", data), VEIL("r", dupe))));
  EZBENCH2("strcasecmp$pure [short dupe]",
           randomize_buf2str_dupe(size, data, dupe),
           EXPROPRIATE(strcasecmp$pure(VEIL("r", data), VEIL("r", dupe))));
  EZBENCH2("strcasecmp [long dupe]", longstringislong_dupe(size, data, dupe),
           EXPROPRIATE(strcasecmp(VEIL("r", data), VEIL("r", dupe))));
  EZBENCH2("strcasecmp$pure [long dupe]",
           longstringislong_dupe(size, data, dupe),
           EXPROPRIATE(strcasecmp$pure(VEIL("r", data), VEIL("r", dupe))));
}
