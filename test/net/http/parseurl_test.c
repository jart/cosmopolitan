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
#include "libc/limits.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/http.h"
#include "net/http/ip.h"
#include "net/http/url.h"

TEST(ParseUrl, testEmpty) {
  struct Url h;
  gc(ParseUrl(0, 0, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.params.n);
  ASSERT_STREQ("", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testFragment) {
  struct Url h;
  gc(ParseUrl("#x", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.path.n);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_BINEQ(u"x", h.fragment.p);
  ASSERT_STREQ("#x", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testFragmentAbsent_isNull) {
  struct Url h;
  gc(ParseUrl("", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.fragment.p);
  ASSERT_EQ(0, h.fragment.n);
  ASSERT_STREQ("", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testFragmentEmpty_isNonNull) {
  struct Url h;
  gc(ParseUrl("#", -1, &h, 0)); /* python's uri parser is wrong here */
  gc(h.params.p);
  ASSERT_NE(0, h.fragment.p);
  ASSERT_EQ(0, h.fragment.n);
  ASSERT_STREQ("#", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testPathFragment) {
  struct Url h;
  gc(ParseUrl("x#y", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('x', h.path.p[0]);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_EQ('y', h.fragment.p[0]);
  ASSERT_STREQ("x#y", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testAbsolutePath) {
  struct Url h;
  gc(ParseUrl("/x/y", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(4, h.path.n);
  ASSERT_BINEQ(u"/x/y", h.path.p);
  ASSERT_STREQ("/x/y", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testRelativePath1) {
  struct Url h;
  gc(ParseUrl("x", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('x', h.path.p[0]);
  ASSERT_STREQ("x", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testOptions) {
  struct Url h;
  gc(ParseUrl("*", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('*', h.path.p[0]);
  ASSERT_STREQ("*", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testRelativePath2) {
  struct Url h;
  gc(ParseUrl("x/y", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(3, h.path.n);
  ASSERT_BINEQ(u"x/y", h.path.p);
  ASSERT_STREQ("x/y", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testRoot) {
  struct Url h;
  gc(ParseUrl("/", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ('/', h.path.p[0]);
  ASSERT_STREQ("/", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testSchemePath) {
  struct Url h;
  gc(ParseUrl("x:y", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_BINEQ(u"x", h.scheme.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_BINEQ(u"y", h.path.p);
  ASSERT_STREQ("x:y", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testSchemeAuthority) {
  struct Url h;
  gc(ParseUrl("x://y", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_EQ('x', h.scheme.p[0]);
  ASSERT_EQ(1, h.host.n);
  ASSERT_EQ('y', h.host.p[0]);
  ASSERT_STREQ("x://y", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testParamsPlus_maybeYes) {
  struct Url h;
  gc(ParseUrl("x?q=hi+there", -1, &h, kUrlPlus));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_BINEQ(u"x", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_EQ(8, h.params.p[0].val.n);
  ASSERT_BINEQ(u"q", h.params.p[0].key.p);
  ASSERT_BINEQ(u"hi there", h.params.p[0].val.p);
  ASSERT_STREQ("x?q=hi%20there", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testParamsPlus_maybeNot) {
  struct Url h;
  gc(ParseUrl("x?q=hi+there", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_BINEQ(u"x", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_EQ(8, h.params.p[0].val.n);
  ASSERT_BINEQ(u"q", h.params.p[0].key.p);
  ASSERT_BINEQ(u"hi+there", h.params.p[0].val.p);
  ASSERT_STREQ("x?q=hi%2Bthere", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testParamsQuestion_doesntTurnIntoSpace) {
  struct Url h;
  gc(ParseUrl("x?+", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_BINEQ(u"x", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_EQ('+', h.params.p[0].key.p[0]);
  ASSERT_STREQ("x?%2B", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testUrl) {
  struct Url h;
  gc(ParseUrl("a://b:B@c:C/d?e#f", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.scheme.n);
  ASSERT_EQ('a', h.scheme.p[0]);
  ASSERT_EQ(1, h.user.n);
  ASSERT_EQ('b', h.user.p[0]);
  ASSERT_EQ(1, h.pass.n);
  ASSERT_EQ('B', h.pass.p[0]);
  ASSERT_STREQ("c", gc(strndup(h.host.p, h.host.n)));
  ASSERT_EQ(1, h.port.n);
  ASSERT_EQ('C', h.port.p[0]);
  ASSERT_EQ(2, h.path.n);
  ASSERT_BINEQ(u"/d", h.path.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(1, h.params.p[0].key.n);
  ASSERT_BINEQ(u"e", h.params.p[0].key.p);
  ASSERT_EQ(0, h.params.p[0].val.n);
  ASSERT_EQ(0, h.params.p[0].val.p);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_BINEQ(u"f", h.fragment.p);
  ASSERT_STREQ("a://b:B@c:C/d?e#f", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testEmptyQueryKeyVal_decodesToEmptyStrings) {
  struct Url h;
  gc(ParseUrl("?=", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(0, h.params.p[0].key.n);
  ASSERT_NE(0, h.params.p[0].key.p);
  ASSERT_EQ(0, h.params.p[0].val.n);
  ASSERT_NE(0, h.params.p[0].val.p);
  ASSERT_STREQ("?=", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testMultipleEquals_goesIntoValue) {
  struct Url h;
  gc(ParseUrl("?==", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.params.n);
  ASSERT_EQ(0, h.params.p[0].key.n);
  ASSERT_NE(0, h.params.p[0].key.p);
  ASSERT_EQ(1, h.params.p[0].val.n);
  ASSERT_EQ('=', h.params.p[0].val.p[0]);
  ASSERT_STREQ("?=%3D", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testUrlWithoutScheme) {
  struct Url h;
  gc(ParseUrl("//b@c/d?e#f", -1, &h, 0));
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
  ASSERT_EQ(0, h.params.p[0].val.n);
  ASSERT_EQ(0, h.params.p[0].val.p);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_BINEQ(u"f", h.fragment.p);
  ASSERT_STREQ("//b@c/d?e#f", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testUrlWithoutUser) {
  struct Url h;
  gc(ParseUrl("a://c/d?e#f", -1, &h, 0));
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
  ASSERT_EQ(0, h.params.p[0].val.n);
  ASSERT_EQ(0, h.params.p[0].val.p);
  ASSERT_EQ(1, h.fragment.n);
  ASSERT_EQ('f', h.fragment.p[0]);
  ASSERT_STREQ("a://c/d?e#f", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testEmptyParams_absentCanBeDiscerned) {
  struct Url h;
  gc(ParseUrl("", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.params.n);
  ASSERT_EQ(NULL, h.params.p);
  gc(ParseUrl("?", -1, &h, 0)); /* python's uri parser is wrong here */
  gc(h.params.p);
  ASSERT_EQ(0, h.params.n);
  ASSERT_NE(NULL, h.params.p);
}

TEST(ParseUrl, testWeirdAmps_areReproducible) {
  struct Url h;
  gc(ParseUrl("?&&", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(3, h.params.n);
  ASSERT_EQ(0, h.params.p[0].key.n);
  ASSERT_NE(0, h.params.p[0].key.p);
  ASSERT_EQ(0, h.params.p[0].val.n);
  ASSERT_EQ(0, h.params.p[0].val.p);
  ASSERT_EQ(0, h.params.p[1].key.n);
  ASSERT_NE(0, h.params.p[1].key.p);
  ASSERT_EQ(0, h.params.p[1].val.n);
  ASSERT_EQ(0, h.params.p[1].val.p);
  ASSERT_EQ(0, h.params.p[2].key.n);
  ASSERT_NE(0, h.params.p[2].key.p);
  ASSERT_EQ(0, h.params.p[2].val.n);
  ASSERT_EQ(0, h.params.p[2].val.p);
  ASSERT_STREQ("?&&", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testOpaquePart_canLetQuestionMarkGoInPath) {
  struct Url h; /* python's uri parser is wrong here */
  gc(ParseUrl("s:o!$%&'()*+,-./09:;=?@AZ_az#fragged", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(26, h.path.n);
  ASSERT_EQ(0, memcmp(h.path.p, "o!$%&'()*+,-./09:;=?@AZ_az", 26));
  ASSERT_EQ(0, h.params.n);
  ASSERT_EQ(NULL, h.params.p);
  ASSERT_STREQ("s:o!$%25&'()*+,-./09:;=%3F@AZ_az#fragged",
               gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testSchemePathWithoutAuthority_paramsAreAllowed) {
  struct Url h;
  gc(ParseUrl("s:/o!$%&'()*+,-./09:;=?@AZ_az#fragged", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(20, h.path.n);
  ASSERT_EQ(0, memcmp(h.path.p, "/o!$%&'()*+,-./09:;=", 20));
  ASSERT_EQ(1, h.params.n);
  ASSERT_STREQ("s:/o!$%25&'()*+,-./09:;=?%40AZ_az#fragged",
               gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testOpaquePart_permitsPercentEncoding) {
  struct Url h;
  gc(ParseUrl("s:%2Fo!$%&'()*+,-./09:;=?@AZ_az#fragged", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(27, h.path.n);
  ASSERT_EQ(0, memcmp(h.path.p, "/o!$%&'()*+,-./09:;=?@AZ_az", 27));
  ASSERT_EQ(0, h.params.n);
  ASSERT_STREQ("s:/o!$%25&\'()*+,-./09:;=%3F@AZ_az#fragged",
               gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testTelephone) {
  struct Url h;
  gc(ParseUrl("tel:+1-212-867-5309", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(15, h.path.n);
  ASSERT_BINEQ(u"+1-212-867-5309", h.path.p);
  ASSERT_STREQ("tel:+1-212-867-5309", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testLolv6) {
  struct Url h;
  gc(ParseUrl("//[::1]:31337", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(3, h.host.n);
  ASSERT_BINEQ(u"::1", h.host.p);
  ASSERT_EQ(5, h.port.n);
  ASSERT_BINEQ(u"31337", h.port.p);
  ASSERT_STREQ("//[::1]:31337", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testLolV6_withoutPort) {
  struct Url h;
  gc(ParseUrl("//[::1]", -1, &h, 0));
  gc(h.params.p);
  ASSERT_STREQ("//[::1]", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testLolv7) {
  struct Url h;
  gc(ParseUrl("//[vf.::1]", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(6, h.host.n);
  ASSERT_BINEQ(u"vf.::1", h.host.p);
  ASSERT_EQ(0, h.port.n);
  ASSERT_EQ(0, h.port.p);
  ASSERT_STREQ("//[vf.::1]", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testLolv7WithoutColon_weCantProduceLegalEncodingSadly) {
  struct Url h;
  gc(ParseUrl("//[v7.7.7.7]", -1, &h, 0));
  gc(h.params.p);
  ASSERT_STREQ("//v7.7.7.7", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testObviouslyIllegalIpLiteral_getsTreatedAsRegName) {
  struct Url h;
  gc(ParseUrl("//[vf.::1%00]", -1, &h, 0));
  gc(h.params.p);
  ASSERT_STREQ("//vf.%3A%3A1%00", gc(EncodeUrl(&h, 0)));
}

TEST(ParseHost, testEmpty) {
  struct Url h = {0};
  gc(ParseHost("", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(0, h.host.n);
  ASSERT_EQ(0, h.port.n);
  ASSERT_NE(0, h.host.p);
  ASSERT_EQ(0, h.port.p);
}

TEST(ParseHost, test) {
  struct Url h = {0};
  gc(ParseHost("foo.example:80", -1, &h));
  gc(h.params.p);
  ASSERT_EQ(11, h.host.n);
  ASSERT_BINEQ(u"foo.example", h.host.p);
  ASSERT_EQ(2, h.port.n);
  ASSERT_BINEQ(u"80", h.port.p);
  ASSERT_STREQ("//foo.example:80", gc(EncodeUrl(&h, 0)));
}

TEST(ParseHost, testObviouslyIllegalIpLiteral_getsTreatedAsRegName) {
  struct Url h = {0};
  gc(ParseHost("[vf.::1%00]", -1, &h));
  gc(h.params.p);
  ASSERT_STREQ("//vf.%3A%3A1%00", gc(EncodeUrl(&h, 0)));
}

TEST(ParseHost, testUnclosedIpv6_doesntSetPort) {
  struct Url h = {0};
  gc(ParseHost("2001:db8:cafe::17", -1, &h));
  gc(h.params.p);
  ASSERT_STREQ("2001:db8:cafe::17", gc(strndup(h.host.p, h.host.n)));
  ASSERT_EQ(0, h.port.n);
  ASSERT_EQ(0, h.port.p);
  ASSERT_STREQ("//2001%3Adb8%3Acafe%3A%3A17", gc(EncodeUrl(&h, 0)));
}

TEST(EncodeUrl, testHostPortPlacedInHostField_ungoodIdea) {
  struct Url h = {0};
  h.host.n = strlen("foo.example:80");
  h.host.p = "foo.example:80";
  ASSERT_STREQ("//foo.example%3A80", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testUrlWithoutParams) {
  struct Url h;
  gc(ParseUrl("a://b@c/d#f", -1, &h, 0));
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
  ASSERT_STREQ("a://b@c/d#f", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testLatin1_doesNothing) {
  struct Url h;
  const char b[1] = {0377};
  gc(ParseUrl(b, 1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ(0, memcmp("\377", h.path.p, 1));
  ASSERT_STREQ("%FF", gc(EncodeUrl(&h, 0)));
}

TEST(ParseRequestUri, testLatin1_expandsMemoryToUtf8) {
  struct Url h;
  const char b[1] = {0377};
  gc(ParseUrl(b, 1, &h, kUrlPlus | kUrlLatin1));
  gc(h.params.p);
  ASSERT_EQ(2, h.path.n);
  ASSERT_EQ(0, memcmp("\303\277", h.path.p, 2));
}

TEST(ParseUrl, testPercentShrinkingMemory) {
  struct Url h;
  gc(ParseUrl("%Ff", 3, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ(0, memcmp("\377", h.path.p, 1));
  ASSERT_STREQ("%FF", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testEscapingWontOverrun) {
  struct Url h;
  char b[1] = {'%'};
  gc(ParseUrl(b, 1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ(0, memcmp("%", h.path.p, 1));
  ASSERT_STREQ("%25", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testBadPercent_getsIgnored) {
  struct Url h;
  gc(ParseUrl("%FZ", 3, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(3, h.path.n);
  ASSERT_EQ(0, memcmp("%FZ", h.path.p, 3));
}

TEST(ParseUrl, testFileUrl) {
  struct Url h;
  gc(ParseUrl("file:///etc/passwd", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(4, h.scheme.n);
  ASSERT_BINEQ(u"file", h.scheme.p);
  ASSERT_EQ(0, h.host.n);
  ASSERT_NE(0, h.host.p);
  ASSERT_EQ(0, h.port.n);
  ASSERT_EQ(0, h.port.p);
  ASSERT_EQ(11, h.path.n);
  ASSERT_BINEQ(u"/etc/passwd", h.path.p);
  ASSERT_STREQ("file:///etc/passwd", gc(EncodeUrl(&h, 0)));
}

TEST(EncodeUrl, testModifyingParseResultAndReencoding_addsStructure) {
  size_t n;
  struct Url h;
  gc(ParseUrl("rel", -1, &h, 0));
  gc(h.params.p);
  h.host.n = 7;
  h.host.p = "justine";
  ASSERT_STREQ("//justine/rel", gc(EncodeUrl(&h, &n)));
  ASSERT_EQ(13, n);
}

TEST(EncodeUrl, testTortureCharacters_doesWhatYouAskItToDoButSchemeCantEscape) {
  size_t n;
  struct Url h;
  memset(&h, 0, sizeof(h));
  h.scheme.n = 1;
  h.scheme.p = "/";
  h.user.n = 1;
  h.user.p = "";
  h.pass.n = 1;
  h.pass.p = "";
  h.host.n = 1;
  h.host.p = "";
  h.port.n = 1;
  h.port.p = "";
  h.path.n = 1;
  h.path.p = "";
  h.params = (struct UrlParams){.n = 1,
                                .p = (struct UrlParam[]){{
                                    .key = (struct UrlView){.n = 1, .p = ""},
                                    .val = (struct UrlView){.n = 1, .p = ""},
                                }}};
  h.fragment.n = 1;
  h.fragment.p = "";
  ASSERT_STREQ("/://%00:%00@%00:%00/%00?%00=%00#%00", gc(EncodeUrl(&h, &n)));
  ASSERT_EQ(35, n);
}

TEST(EncodeUrl, testUserPassPort_allDependOnHostNonAbsence) {
  struct Url h;
  memset(&h, 0, sizeof(h));
  h.scheme.n = 1;
  h.scheme.p = "/";
  h.user.n = 1;
  h.user.p = "";
  h.pass.n = 1;
  h.pass.p = "";
  h.host.n = 0;
  h.host.p = 0;
  h.port.n = 1;
  h.port.p = "";
  h.path.n = 1;
  h.path.p = "";
  h.params = (struct UrlParams){.n = 1,
                                .p = (struct UrlParam[]){{
                                    .key = (struct UrlView){.n = 1, .p = ""},
                                    .val = (struct UrlView){.n = 1, .p = ""},
                                }}};
  h.fragment.n = 1;
  h.fragment.p = "";
  ASSERT_STREQ("/:%00?%00=%00#%00", gc(EncodeUrl(&h, 0)));
}

TEST(EncodeUrl, testEmptyRegName_isLegal) {
  struct Url h;
  memset(&h, 0, sizeof(h));
  h.scheme.n = 1;
  h.scheme.p = "/";
  h.user.n = 1;
  h.user.p = "";
  h.pass.n = 1;
  h.pass.p = "";
  h.host.n = 0;
  h.host.p = "";
  h.port.n = 1;
  h.port.p = "";
  h.path.n = 1;
  h.path.p = "";
  h.params = (struct UrlParams){.n = 1,
                                .p = (struct UrlParam[]){{
                                    .key = (struct UrlView){.n = 1, .p = ""},
                                    .val = (struct UrlView){.n = 1, .p = ""},
                                }}};
  h.fragment.n = 1;
  h.fragment.p = "";
  ASSERT_STREQ("/://%00:%00@:%00/%00?%00=%00#%00", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testEmptyScheme_isNotPossible) {
  struct Url h;
  gc(ParseUrl(":", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.scheme.n);
  ASSERT_EQ(0, h.scheme.p);
  ASSERT_EQ(1, h.path.n);
  ASSERT_EQ(':', h.path.p[0]);
  ASSERT_STREQ(":", gc(EncodeUrl(&h, 0)));
  gc(ParseUrl("://hi", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.scheme.n);
  ASSERT_EQ(0, h.scheme.p);
  ASSERT_EQ(5, h.path.n);
  ASSERT_BINEQ(u"://hi", h.path.p);
  ASSERT_STREQ("://hi", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testDataUri) {
  struct Url h;
  gc(ParseUrl("data:image/png;base64,09AZaz+/==", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.host.n);
  ASSERT_EQ(0, h.host.p);
  ASSERT_EQ(4, h.scheme.n);
  ASSERT_BINEQ(u"data", h.scheme.p);
  ASSERT_EQ(27, h.path.n);
  ASSERT_BINEQ(u"image/png;base64,09AZaz+/==", h.path.p);
  ASSERT_STREQ("data:image/png;base64,09AZaz+/==", gc(EncodeUrl(&h, 0)));
}

TEST(ParseUrl, testBadSchemeCharacter_parserAssumesItsPath) {
  struct Url h;
  gc(ParseUrl("fil\e://hi", -1, &h, 0));
  gc(h.params.p);
  ASSERT_EQ(0, h.scheme.n);
  ASSERT_EQ(0, h.scheme.p);
  ASSERT_EQ(9, h.path.n);
  ASSERT_BINEQ(u"fil←://hi", h.path.p);
  ASSERT_STREQ("fil%1B://hi", gc(EncodeUrl(&h, 0)));
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
  ASSERT_NE(0, h.p[1].key.p);
  ASSERT_EQ(0, h.p[1].val.n);
  ASSERT_EQ(0, h.p[1].val.p);
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
    free(ParseUrl(B, 8, &h, kUrlPlus | kUrlLatin1));
    free(h.params.p);
  }
}

void A(void) {
  struct UrlParams h = {0};
  free(ParseParams(kHyperion, kHyperionSize, &h));
  free(h.p);
}

BENCH(ParseUrl, bench) {
  struct Url h;
  EZBENCH2("ParseParams hyperion", donothing, A());
  EZBENCH2("ParseUrl a", donothing, free(ParseUrl("a", -1, &h, false)));
  EZBENCH2("ParseUrl a://b@c/d#f", donothing,
           free(ParseUrl("a://b@c/d#f", -1, &h, false)));
  EZBENCH2("ParseUrl a://b@c/d?z#f", donothing, ({
             free(ParseUrl("a://b@c/?zd#f", -1, &h, 0));
             free(h.params.p);
           }));
  EZBENCH2("ParseHost", donothing, free(ParseHost("127.0.0.1:34832", 15, &h)));
  EZBENCH2("ParseIp", donothing, ParseIp("127.0.0.1", 9));
}

BENCH(EncodeUrl, bench) {
  struct Url h;
  gc(ParseUrl("a", -1, &h, 0));
  EZBENCH2("EncodeUrl a", donothing, free(EncodeUrl(&h, 0)));
  gc(ParseUrl("a://b@c/d#f", -1, &h, 0));
  EZBENCH2("EncodeUrl a://b@c/d#f", donothing, free(EncodeUrl(&h, 0)));
  gc(ParseUrl("a://b@c/?zd#f", -1, &h, 0));
  gc(h.params.p);
  EZBENCH2("EncodeUrl a://b@c/d?z#f", donothing, free(EncodeUrl(&h, 0)));
  gc(ParseUrl(kHyperion, kHyperionSize, &h, 0));
  gc(h.params.p);
  EZBENCH2("EncodeUrl hyperion", donothing, free(EncodeUrl(&h, 0)));
}
