/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/http.h"

ssize_t EzUnchunk(char *p, size_t n, size_t *l) {
  struct HttpUnchunker u = {0};
  return Unchunk(&u, p, n, l);
}

TEST(Unchunk, emptyInput_needsMoreData) {
  EXPECT_EQ(0, EzUnchunk(0, 0, 0));
}

TEST(Unchunk, tooShort_needsMoreData) {
  char s[] = "0\r\n";
  EXPECT_EQ(0, EzUnchunk(s, strlen(s), 0));
}

TEST(Unchunk, missingLength_needAtLeastOneSizeDigit) {
  char s[] = "\r\n\r\n";
  EXPECT_EQ(-1, EzUnchunk(s, strlen(s), 0));
}

TEST(Unchunk, wrongLength_breaksExpectations) {
  char s[] = "3\r\nX\r\n0\r\n\r\n";
  EXPECT_EQ(-1, EzUnchunk(s, strlen(s), 0));
}

TEST(Unchunk, maliciousLength_needsMoreData) {
  char s[] = "9\r\nX\r\n0\r\n\r\n";
  EXPECT_EQ(0, EzUnchunk(s, strlen(s), 0));
}

TEST(Unchunk, overflowsAsCorrectLength_detectsOverflow) {
  char s[] = "10000000000000001\r\nX\r\n0\r\n\r\n";
  EXPECT_EQ(-1, EzUnchunk(s, strlen(s), 0));
}

TEST(Unchunk, emptyContent) {
  size_t l;
  ssize_t n;
  char s[] = "0\r\n\r\n";
  EXPECT_EQ(5, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(0, l);
  EXPECT_STREQ("", s);
  EXPECT_STREQ("", s + n);
}

TEST(Unchunk, weirdlyEmptyContent) {
  size_t l;
  ssize_t n;
  char s[] = "00000000000\r\n\r\nextra";
  EXPECT_NE(-1, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(0, l);
  EXPECT_STREQ("", s);
  EXPECT_STREQ("extra", s + n);
}

TEST(Unchunk, smallest) {
  size_t l;
  ssize_t n;
  char s[] = "1\r\nX\r\n0\r\n\r\n";
  EXPECT_NE(-1, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(1, l);
  EXPECT_STREQ("X", s);
  EXPECT_STREQ("", s + n);
}

TEST(Unchunk, testBasicIdea) {
  size_t l;
  ssize_t n;
  char s[] = "\
7\r\n\
Mozilla\r\n\
1e\r\n\
DevelopersDevelopersDevelopers\r\n\
7\r\n\
Network\r\n\
0\r\n\
\r\n\
extra guff";
  EXPECT_NE(-1, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(0x7 + 0x1e + 0x7, l);
  EXPECT_STREQ("MozillaDevelopersDevelopersDevelopersNetwork", s);
  EXPECT_STREQ("extra guff", s + n);
}

TEST(Unchunk, testTrailers_areIgnored) {
  size_t l;
  ssize_t n;
  char s[] = "\
7\r\n\
Mozilla\r\n\
001E\r\n\
DevelopersDevelopersDevelopers\r\n\
7\r\n\
Network\r\n\
0\r\n\
X-Lol: Cat\r\n\
X-Cat: Lol\r\n\
\r\n\
extra guff";
  EXPECT_NE(-1, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(0x7 + 0x1e + 0x7, l);
  EXPECT_STREQ("MozillaDevelopersDevelopersDevelopersNetwork", s);
  EXPECT_STREQ("extra guff", s + n);
}

TEST(Unchunk, testChunkExtensions_areIgnored) {
  size_t l;
  ssize_t n;
  char s[] = "\
7;hey=ho;yo\r\n\
Mozilla\r\n\
1e;hey=ho;yo\r\n\
DevelopersDevelopersDevelopers\r\n\
7;hey=ho;yo\r\n\
Network\r\n\
0\r\n\
\r\n\
extra guff";
  EXPECT_NE(-1, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(0x7 + 0x1e + 0x7, l);
  EXPECT_STREQ("MozillaDevelopersDevelopersDevelopersNetwork", s);
  EXPECT_STREQ("extra guff", s + n);
}

TEST(Unchunk, testNewlines_areFlexible) {
  size_t l;
  ssize_t n;
  char s[] = "\
7\n\
Mozilla\n\
1e\n\
DevelopersDevelopersDevelopers\n\
7\n\
Network\n\
0\n\
X-Lol: Cat\n\
X-Cat: Lol\n\
\n\
extra guff";
  EXPECT_NE(-1, (n = EzUnchunk(s, strlen(s), &l)));
  EXPECT_EQ(0x7 + 0x1e + 0x7, l);
  EXPECT_STREQ("MozillaDevelopersDevelopersDevelopersNetwork", s);
  EXPECT_STREQ("extra guff", s + n);
}
