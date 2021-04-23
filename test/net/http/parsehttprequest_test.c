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
#include "libc/bits/bits.h"
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/http.h"

struct HttpRequest req[1];

static char *slice(const char *m, struct HttpRequestSlice s) {
  char *p;
  p = xmalloc(s.b - s.a + 1);
  memcpy(p, m + s.a, s.b - s.a);
  p[s.b - s.a] = 0;
  return p;
}

void SetUp(void) {
  InitHttpRequest(req);
}

void TearDown(void) {
  DestroyHttpRequest(req);
}

TEST(ParseHttpRequest, soLittleState) {
  ASSERT_LE(sizeof(struct HttpRequest), 512);
}

TEST(ParseHttpRequest, testEmpty_tooShort) {
  EXPECT_EQ(0, ParseHttpRequest(req, "", 0));
}

TEST(ParseHttpRequest, testTooShort) {
  EXPECT_EQ(0, ParseHttpRequest(req, "\r\n", 2));
}

TEST(ParseHttpRequest, testNoHeaders) {
  static const char m[] = "GET /foo HTTP/1.0\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/foo", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
}

TEST(ParseHttpRequest, testSomeHeaders) {
  static const char m[] = "\
POST /foo?bar%20hi HTTP/1.0\r\n\
Host: foo.example\r\n\
Content-Length: 0\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpPost, req->method);
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}

TEST(ParseHttpRequest, testHttp101) {
  static const char m[] = "GET / HTTP/1.1\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/", gc(slice(m, req->uri)));
  EXPECT_EQ(11, req->version);
}

TEST(ParseHttpRequest, testHttp100) {
  static const char m[] = "GET / HTTP/1.0\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
}

TEST(ParseHttpRequest, testUnknownMethod_canBeUsedIfYouWant) {
  static const char m[] = "#%*+_^ / HTTP/1.0\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_FALSE(req->method);
  EXPECT_STREQ("WUT", kHttpMethod[req->method]);
  EXPECT_STREQ("#%*+_^", gc(slice(m, req->xmethod)));
}

TEST(ParseHttpRequest, testIllegalMethod) {
  static const char m[] = "ehd@oruc / HTTP/1.0\r\n\r\n";
  EXPECT_EQ(-1, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("WUT", kHttpMethod[req->method]);
}

TEST(ParseHttpRequest, testIllegalMethodCasing_weAllowItAndPreserveIt) {
  static const char m[] = "get / HTTP/1.0\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("GET", kHttpMethod[req->method]);
  EXPECT_STREQ("get", gc(slice(m, req->xmethod)));
}

TEST(ParseHttpRequest, testEmptyMethod_isntAllowed) {
  static const char m[] = " / HTTP/1.0\r\n\r\n";
  EXPECT_EQ(-1, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("WUT", kHttpMethod[req->method]);
}

TEST(ParseHttpRequest, testEmptyUri_isntAllowed) {
  static const char m[] = "GET  HTTP/1.0\r\n\r\n";
  EXPECT_EQ(-1, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("GET", kHttpMethod[req->method]);
}

TEST(ParseHttpRequest, testHttp09) {
  static const char m[] = "GET /\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/", gc(slice(m, req->uri)));
  EXPECT_EQ(9, req->version);
}

TEST(ParseHttpRequest, testLeadingLineFeeds_areIgnored) {
  static const char m[] = "\
\r\n\
GET /foo?bar%20hi HTTP/1.0\r\n\
User-Agent: hi\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
}

TEST(ParseHttpRequest, testLineFolding_isRejected) {
  static const char m[] = "\
GET /foo?bar%20hi HTTP/1.0\r\n\
User-Agent: hi\r\n\
 there\r\n\
\r\n";
  EXPECT_EQ(-1, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(EBADMSG, errno);
}

TEST(ParseHttpRequest, testEmptyHeaderName_isRejected) {
  static const char m[] = "\
GET /foo?bar%20hi HTTP/1.0\r\n\
User-Agent: hi\r\n\
: hi\r\n\
\r\n";
  EXPECT_EQ(-1, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(EBADMSG, errno);
}

TEST(ParseHttpRequest, testUnixNewlines) {
  static const char m[] = "\
POST /foo?bar%20hi HTTP/1.0\n\
Host: foo.example\n\
Content-Length: 0\n\
\n\
\n";
  EXPECT_EQ(strlen(m) - 1, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpPost, req->method);
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}

TEST(ParseHttpRequest, testChromeMessage) {
  static const char m[] = "\
GET /tool/net/redbean.png HTTP/1.1\r\n\
Host: 10.10.10.124:8080\r\n\
Connection: keep-alive\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36\r\n\
DNT:  \t1\r\n\
Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n\
Referer: http://10.10.10.124:8080/\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/tool/net/redbean.png", gc(slice(m, req->uri)));
  EXPECT_EQ(11, req->version);
  EXPECT_STREQ("10.10.10.124:8080", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("1", gc(slice(m, req->headers[kHttpDnt])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpExpect])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpExpect])));
}

TEST(ParseHttpRequest, testExtendedHeaders) {
  static const char m[] = "\
GET /foo?bar%20hi HTTP/1.0\r\n\
X-User-Agent: hi\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  ASSERT_EQ(1, req->xheaders.n);
  EXPECT_STREQ("X-User-Agent", gc(slice(m, req->xheaders.p[0].k)));
  EXPECT_STREQ("hi", gc(slice(m, req->xheaders.p[0].v)));
}

TEST(ParseHttpRequest, testNormalHeaderOnMultipleLines_getsOverwritten) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Content-Type: text/html\r\n\
Content-Type: text/plain\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("text/plain", gc(slice(m, req->headers[kHttpContentType])));
  ASSERT_EQ(0, req->xheaders.n);
}

TEST(ParseHttpRequest, testCommaSeparatedOnMultipleLines_becomesLinear) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Accept: text/html\r\n\
Accept: text/plain\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("text/html", gc(slice(m, req->headers[kHttpAccept])));
  ASSERT_EQ(1, req->xheaders.n);
  EXPECT_STREQ("Accept", gc(slice(m, req->xheaders.p[0].k)));
  EXPECT_STREQ("text/plain", gc(slice(m, req->xheaders.p[0].v)));
}

TEST(HeaderHas, testHeaderSpansMultipleLines) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Accept-Encoding: deflate\r\n\
ACCEPT-ENCODING: gzip\r\n\
ACCEPT-encoding: bzip2\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "gzip", -1));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "deflate", -1));
  EXPECT_FALSE(HeaderHas(req, m, kHttpAcceptEncoding, "funzip", -1));
}

TEST(HeaderHas, testHeaderOnSameLIne) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Accept-Encoding: deflate, gzip, bzip2\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "gzip", -1));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "deflate", -1));
  EXPECT_FALSE(HeaderHas(req, m, kHttpAcceptEncoding, "funzip", -1));
}

TEST(ParseHttpRequest, testHeaderValuesWithWhitespace_getsTrimmed) {
  static const char m[] = "\
OPTIONS * HTTP/1.0\r\n\
User-Agent:  \t hi there \t \r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_STREQ("hi there", gc(slice(m, req->headers[kHttpUserAgent])));
  EXPECT_STREQ("*", gc(slice(m, req->uri)));
}

TEST(ParseHttpRequest, testAbsentHost_setsSliceToZero) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(0, req->headers[kHttpHost].a);
  EXPECT_EQ(0, req->headers[kHttpHost].b);
}

TEST(ParseHttpRequest, testEmptyHost_setsSliceToNonzeroValue) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Host:\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_NE(0, req->headers[kHttpHost].a);
  EXPECT_EQ(req->headers[kHttpHost].a, req->headers[kHttpHost].b);
}

TEST(ParseHttpRequest, testEmptyHost2_setsSliceToNonzeroValue) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Host:    \r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EXPECT_NE(0, req->headers[kHttpHost].a);
  EXPECT_EQ(req->headers[kHttpHost].a, req->headers[kHttpHost].b);
}

TEST(IsMimeType, test) {
  ASSERT_TRUE(IsMimeType("text/plain", -1, "text/plain"));
  ASSERT_TRUE(IsMimeType("TEXT/PLAIN", -1, "text/plain"));
  ASSERT_TRUE(IsMimeType("TEXT/PLAIN ", -1, "text/plain"));
  ASSERT_TRUE(IsMimeType("text/plain; charset=utf-8", -1, "text/plain"));
  ASSERT_FALSE(IsMimeType("TEXT/PLAI ", -1, "text/plain"));
  ASSERT_FALSE(IsMimeType("", -1, "text/plain"));
}

void DoTiniestHttpRequest(void) {
  static const char m[] = "\
GET /\r\n\
\r\n";
  InitHttpRequest(req);
  ParseHttpRequest(req, m, sizeof(m));
  DestroyHttpRequest(req);
}

void DoTinyHttpRequest(void) {
  static const char m[] = "\
GET /\r\n\
Accept-Encoding: gzip\r\n\
\r\n";
  InitHttpRequest(req);
  ParseHttpRequest(req, m, sizeof(m));
  DestroyHttpRequest(req);
}

void DoStandardChromeRequest(void) {
  static const char m[] = "\
GET /tool/net/redbean.png HTTP/1.1\r\n\
Host: 10.10.10.124:8080\r\n\
Connection: keep-alive\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36\r\n\
DNT:  \t1   \r\n\
Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n\
Referer: http://10.10.10.124:8080/\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
\r\n";
  InitHttpRequest(req);
  CHECK_EQ(sizeof(m) - 1, ParseHttpRequest(req, m, sizeof(m)));
  DestroyHttpRequest(req);
}

void DoUnstandardChromeRequest(void) {
  static const char m[] = "\
GET /tool/net/redbean.png HTTP/1.1\r\n\
X-Host: 10.10.10.124:8080\r\n\
X-Connection: keep-alive\r\n\
X-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36\r\n\
X-DNT:  \t1   \r\n\
X-Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n\
X-Referer: http://10.10.10.124:8080/\r\n\
X-Accept-Encoding: gzip, deflate\r\n\
X-Accept-Language: en-US,en;q=0.9\r\n\
\r\n";
  InitHttpRequest(req);
  CHECK_EQ(sizeof(m) - 1, ParseHttpRequest(req, m, sizeof(m)));
  DestroyHttpRequest(req);
}

BENCH(ParseHttpRequest, bench) {
  EZBENCH2("DoTiniestHttpRequest", donothing, DoTiniestHttpRequest());
  EZBENCH2("DoTinyHttpRequest", donothing, DoTinyHttpRequest());
  EZBENCH2("DoStandardChromeRequest", donothing, DoStandardChromeRequest());
  EZBENCH2("DoUnstandardChromeRequest", donothing, DoUnstandardChromeRequest());
}

BENCH(HeaderHas, bench) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
X-In-Your-Way-A: a\r\n\
X-In-Your-Way-B: b\r\n\
X-In-Your-Way-C: b\r\n\
Accept-Encoding: deflate\r\n\
ACCEPT-ENCODING: gzip\r\n\
ACCEPT-encoding: bzip2\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpRequest(req, m, strlen(m)));
  EZBENCH2("HeaderHas text/plain", donothing,
           HeaderHas(req, m, kHttpAccept, "text/plain", 7));
  EZBENCH2("HeaderHas deflate", donothing,
           HeaderHas(req, m, kHttpAcceptEncoding, "deflate", 7));
  EZBENCH2("HeaderHas gzip", donothing,
           HeaderHas(req, m, kHttpAcceptEncoding, "gzip", 4));
  EZBENCH2("IsMimeType", donothing,
           IsMimeType("text/plain; charset=utf-8", -1, "text/plain"));
}
