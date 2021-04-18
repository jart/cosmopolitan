/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/limits.h"
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/url.h"

TEST(ParseRequestUri, testEmpty) {
  struct Url h;
  gc(ParseRequestUri(0, 0, &h));
  gc(h.params.p);
  ASSERT_EQ(0, h.params.n);
}

TEST(ParseRequestUri, testFragment) {
  struct Url h;
  gc(ParseRequestUri("#x", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(0, h.path.n);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_BINEQ(u"x", h.fragment.p);
}

TEST(ParseRequestUri, testFragmentAbsent_isNull) {
  struct Url h;
  gc(ParseRequestUri("", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(0, h.fragment.p);
  ASSERT_EQ(0, h.fragment.n);
}

TEST(ParseRequestUri, testFragmentEmpty_isNonNull) {
  struct Url h;
  gc(ParseRequestUri("#", -1, &h));
  gc(h.params.p);
  ASSERT_NE(0, h.fragment.p);
  ASSERT_EQ(0, h.fragment.n);
}

TEST(ParseRequestUri, testPathFragment) {
  struct Url h;
  gc(ParseRequestUri("x#y", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('x', h.path.p[0]);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_EQ('y', h.fragment.p[0]);
}

TEST(ParseRequestUri, testAbsolutePath) {
  struct Url h;
  gc(ParseRequestUri("/x/y", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(4, h.path.n);
  ASSERT_BINEQ(u"/x/y", h.path.p);
}

TEST(ParseRequestUri, testRelativePath1) {
  struct Url h;
  gc(ParseRequestUri("x", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('x', h.path.p[0]);
}

TEST(ParseRequestUri, testOptions) {
  struct Url h;
  gc(ParseRequestUri("*", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('*', h.path.p[0]);
}

TEST(ParseRequestUri, testRelativePath2) {
  struct Url h;
  gc(ParseRequestUri("x/y", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(3, h.path.n);
  ASSERT_BINEQ(u"x/y", h.path.p);
}

TEST(ParseRequestUri, testRoot) {
  struct Url h;
  gc(ParseRequestUri("/", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('/', h.path.p[0]);
}

TEST(ParseRequestUri, testSchemePath) {
  struct Url h;
  gc(ParseRequestUri("x:y", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_BINEQ(u"x", h.scheme.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_BINEQ(u"y", h.path.p);
}

TEST(ParseRequestUri, testSchemeAuthority) {
  struct Url h;
  gc(ParseRequestUri("x://y", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_EQ('x', h.scheme.p[0]);
  ASSERT_EQ(1, h.host.n);
  ASSERT_EQ('y', h.host.p[0]);
}

TEST(ParseRequestUri, testParamsQuestion_doesntTurnIntoSpace) {
  struct Url h;
  gc(ParseRequestUri("x?+", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_BINEQ(u"x", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_EQ('+', h.params.p[0].key.p[0]);
}

TEST(ParseRequestUri, testUrl) {
  struct Url h;
  gc(ParseRequestUri("a://b:B@c:C/d?e#f", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_EQ('a', h.scheme.p[0]);
  ASSERT_EQ(1, h.user.n);
  ASSERT_EQ('b', h.user.p[0]);
  ASSERT_EQ(1, h.pass.n);
  ASSERT_EQ('B', h.pass.p[0]);
  ASSERT_EQ(1, h.host.n);
  ASSERT_EQ('c', h.host.p[0]);
  ASSERT_EQ(1, h.port.n);
  ASSERT_EQ('C', h.port.p[0]);
  ASSERT_EQ(2, h.path.n);
  ASSERT_BINEQ(u"/d", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_BINEQ(u"e", h.params.p[0].key.p);
  ASSERT_EQ(SIZE_MAX, h.params.p[0].val.n);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_BINEQ(u"f", h.fragment.p);
}

TEST(ParseRequestUri, testUrlWithoutScheme) {
  struct Url h;
  gc(ParseRequestUri("//b@c/d?e#f", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(0, h.scheme.n);
  ASSERT_EQ(1, h.user.n);
  ASSERT_EQ('b', h.user.p[0]);
  ASSERT_EQ(1, h.host.n);
  ASSERT_EQ('c', h.host.p[0]);
  ASSERT_EQ(2, h.path.n);
  ASSERT_BINEQ(u"/d", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_BINEQ(u"e", h.params.p[0].key.p);
  ASSERT_EQ(SIZE_MAX, h.params.p[0].val.n);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_BINEQ(u"f", h.fragment.p);
}

TEST(ParseRequestUri, testUrlWithoutUser) {
  struct Url h;
  gc(ParseRequestUri("a://c/d?e#f", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_EQ('a', h.scheme.p[0]);
  ASSERT_EQ(0, h.user.n);
  ASSERT_EQ(0, h.pass.n);
  ASSERT_EQ(1, h.host.n);
  ASSERT_EQ('c', h.host.p[0]);
  ASSERT_EQ(0, h.port.n);
  ASSERT_EQ(2, h.path.n);
  ASSERT_BINEQ(u"/d", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_EQ('e', h.params.p[0].key.p[0]);
  ASSERT_EQ(SIZE_MAX, h.params.p[0].val.n);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_EQ('f', h.fragment.p[0]);
}

TEST(ParseRequestUri, testLolv6) {
  struct Url h;
  gc(ParseRequestUri("//[::1]:31337", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(3, h.host.n);
  ASSERT_BINEQ(u"::1", h.host.p);
  ASSERT_EQ(5, h.port.n);
  ASSERT_BINEQ(u"31337", h.port.p);
}

TEST(ParseRequestUri, testUrlWithoutParams) {
  struct Url h;
  gc(ParseRequestUri("a://b@c/d#f", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_EQ('a', h.scheme.p[0]);
  ASSERT_EQ(1, h.user.n);
  ASSERT_EQ('b', h.user.p[0]);
  ASSERT_EQ(1, h.host.n);
  ASSERT_EQ('c', h.host.p[0]);
  ASSERT_EQ(2, h.path.n);
  ASSERT_BINEQ(u"/d", h.path.p);
  ASSERT_EQ(0, h.params.n);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_EQ('f', h.fragment.p[0]);
}

TEST(ParseUrl, testLatin1_doesNothing) {
  struct Url h;
  const char b[1] = {0377};
  gc(ParseUrl(b, 1, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ(0, memcmp("\377", h.path.p, 1));
}

TEST(ParseRequestUri, testLatin1_expandsMemoryToUtf8) {
  struct Url h;
  const char b[1] = {0377};
  gc(ParseRequestUri(b, 1, &h));
  gc(h.params.p);
  ASSERT_EQ(2, h.path.n);
  ASSERT_EQ(0, memcmp("\303\277", h.path.p, 2));
}

TEST(ParseRequestUri, testPercentShrinkingMemory) {
  struct Url h;
  gc(ParseRequestUri("%Ff", 3, &h));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ(0, memcmp("\377", h.path.p, 1));
}

TEST(ParseRequestUri, testBadPercent_getsIgnored) {
  struct Url h;
  gc(ParseRequestUri("%FZ", 3, &h));
  gc(h.params.p);
  ASSERT_EQ(3, h.path.n);
  ASSERT_EQ(0, memcmp("%FZ", h.path.p, 3));
}

TEST(ParseRequestUri, testFileUrl) {
  struct Url h;
  gc(ParseRequestUri("file:///etc/passwd", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(4, h.scheme.n);
  ASSERT_BINEQ(u"file", h.scheme.p);
  ASSERT_EQ(11, h.path.n);
  ASSERT_BINEQ(u"/etc/passwd", h.path.p);
}

TEST(ParseRequestUri, testZipUri2) {
  struct Url h;
  gc(ParseRequestUri("zip:etc/passwd", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(3, h.scheme.n);
  ASSERT_BINEQ(u"zip", h.scheme.p);
  ASSERT_EQ(10, h.path.n);
  ASSERT_BINEQ(u"etc/passwd", h.path.p);
}

TEST(ParseParams, testEmpty) {
  struct UrlParams h = {0};
  gc(ParseParams(0, 0, &h));
  gc(h.p);
  ASSERT_EQ(0, h.n);
}

TEST(ParseParams, test) {
  struct UrlParams h = {0};
  gc(ParseParams("a=b&c&x+y%7A=", -1, &h));
  gc(h.p);
  ASSERT_EQ(3, h.n);
  ASSERT_EQ(1, h.p[0].key.n);
  ASSERT_EQ(1, h.p[0].val.n);
  ASSERT_EQ(1, h.p[1].key.n);
  ASSERT_EQ(SIZE_MAX, h.p[1].val.n);
  ASSERT_EQ(4, h.p[2].key.n);
  ASSERT_EQ(0, h.p[2].val.n);
  EXPECT_EQ('a', h.p[0].key.p[0]);
  EXPECT_EQ('b', h.p[0].val.p[0]);
  EXPECT_EQ('c', h.p[1].key.p[0]);
  EXPECT_BINEQ(u"x yz", h.p[2].key.p);
}

TEST(ParseParams, testLatin1_doesNothing) {
  struct UrlParams h = {0};
  gc(ParseParams("\200", -1, &h));
  gc(h.p);
  ASSERT_EQ(1, h.n);
  ASSERT_EQ(1, h.p[0].key.n);
  ASSERT_EQ(0200, h.p[0].key.p[0] & 255);
}

TEST(ParseParams, testUtf8_doesNothing) {
  struct UrlParams h = {0};
  gc(ParseParams("\300\200", -1, &h));
  gc(h.p);
  ASSERT_EQ(1, h.n);
  ASSERT_EQ(2, h.p[0].key.n);
  ASSERT_EQ(0300, h.p[0].key.p[0] & 255);
  ASSERT_EQ(0200, h.p[0].key.p[1] & 255);
}

TEST(ParseRequestUri, fuzz) {
  int i, j;
  struct Url h;
  char B[13], C[] = "/:#?%[]:@&=abc123xyz\200\300";
  for (i = 0; i < 1024; ++i) {
    for (j = 0; j < sizeof(B); ++j) {
      B[j] = C[rand() % sizeof(C)];
    }
    free(ParseRequestUri(B, 8, &h));
    free(h.params.p);
  }
}

void A(void) {
  struct UrlParams h = {0};
  free(ParseParams(kHyperion, kHyperionSize, &h));
  free(h.p);
}

BENCH(url, bench) {
  struct Url h;
  EZBENCH2("ParseParams", donothing, A());
  EZBENCH2("URI a", donothing, free(ParseRequestUri("a", -1, &h)));
  EZBENCH2("URI a://b@c/d#f", donothing,
           free(ParseRequestUri("a://b@c/d#f", -1, &h)));
  EZBENCH2("URI a://b@c/d?z#f", donothing, ({
             free(ParseRequestUri("a://b@c/?zd#f", -1, &h));
             free(h.params.p);
           }));
}
