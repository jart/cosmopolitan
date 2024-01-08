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
#include "libc/errno.h"
#include "libc/log/check.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/http.h"

struct HttpMessage req[1];

static char *slice(const char *m, struct HttpSlice s) {
  char *p;
  p = xmalloc(s.b - s.a + 1);
  memcpy(p, m + s.a, s.b - s.a);
  p[s.b - s.a] = 0;
  return p;
}

void TearDown(void) {
  DestroyHttpMessage(req);
}

TEST(ParseHttpMessage, soLittleState) {
  InitHttpMessage(req, kHttpRequest);
  ASSERT_LE(sizeof(struct HttpMessage), 512);
}

TEST(ParseHttpMessage, testEmpty_tooShort) {
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(0, ParseHttpMessage(req, "", 0));
}

TEST(ParseHttpMessage, testTooShort) {
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(0, ParseHttpMessage(req, "\r\n", 2));
}

TEST(ParseHttpMessage, testNoHeaders) {
  static const char m[] = "GET /foo HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/foo", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
}

TEST(ParseHttpMessage, testSomeHeaders) {
  static const char m[] = "\
POST /foo?bar%20hi HTTP/1.0\r\n\
Host: foo.example\r\n\
Content-Length: 0\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpPost, req->method);
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}

TEST(ParseHttpMessage, testHttp101) {
  static const char m[] = "GET / HTTP/1.1\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/", gc(slice(m, req->uri)));
  EXPECT_EQ(11, req->version);
}

TEST(ParseHttpMessage, testHttp100) {
  static const char m[] = "GET / HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
}

TEST(ParseHttpMessage, testUnknownMethod_canBeUsedIfYouWant) {
  static const char m[] = "#%*+_^ / HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_FALSE(req->method);
  EXPECT_STREQ("WUT", kHttpMethod[req->method]);
  EXPECT_STREQ("#%*+_^", gc(slice(m, req->xmethod)));
}

TEST(ParseHttpMessage, testIllegalMethod) {
  static const char m[] = "ehd@oruc / HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(-1, ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("WUT", kHttpMethod[req->method]);
}

TEST(ParseHttpMessage, testIllegalMethodCasing_weAllowItAndPreserveIt) {
  static const char m[] = "get / HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("GET", kHttpMethod[req->method]);
  EXPECT_STREQ("get", gc(slice(m, req->xmethod)));
}

TEST(ParseHttpMessage, testEmptyMethod_isntAllowed) {
  static const char m[] = " / HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(-1, ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("WUT", kHttpMethod[req->method]);
}

TEST(ParseHttpMessage, testEmptyUri_isntAllowed) {
  static const char m[] = "GET  HTTP/1.0\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(-1, ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("GET", kHttpMethod[req->method]);
}

TEST(ParseHttpMessage, testHttp09) {
  static const char m[] = "GET /\r\n\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/", gc(slice(m, req->uri)));
  EXPECT_EQ(9, req->version);
}

TEST(ParseHttpMessage, testTinyResponse) {
  static const char m[] = "HTTP/1.1 429 \r\n\r\n";
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(429, req->status);
  EXPECT_STREQ("", gc(slice(m, req->message)));
}

TEST(ParseHttpMessage, testLeadingLineFeeds_areIgnored) {
  static const char m[] = "\
\r\n\
GET /foo?bar%20hi HTTP/1.0\r\n\
User-Agent: hi\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
}

TEST(ParseHttpMessage, testLineFolding_isRejected) {
  static const char m[] = "\
GET /foo?bar%20hi HTTP/1.0\r\n\
User-Agent: hi\r\n\
 there\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(-1, ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(EBADMSG, errno);
}

TEST(ParseHttpMessage, testEmptyHeaderName_isRejected) {
  static const char m[] = "\
GET /foo?bar%20hi HTTP/1.0\r\n\
User-Agent: hi\r\n\
: hi\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(-1, ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(EBADMSG, errno);
}

TEST(ParseHttpMessage, testUnixNewlines) {
  static const char m[] = "\
POST /foo?bar%20hi HTTP/1.0\n\
Host: foo.example\n\
Content-Length: 0\n\
\n\
\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m) - 1, ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpPost, req->method);
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
  EXPECT_EQ(10, req->version);
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}

TEST(ParseHttpMessage, testChromeMessage) {
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
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/tool/net/redbean.png", gc(slice(m, req->uri)));
  EXPECT_EQ(11, req->version);
  EXPECT_STREQ("10.10.10.124:8080", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("1", gc(slice(m, req->headers[kHttpDnt])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpExpect])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpExpect])));
}

TEST(ParseHttpMessage, testExtendedHeaders) {
  static const char m[] = "\
GET /foo?bar%20hi HTTP/1.0\r\n\
X-User-Agent: hi\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  ASSERT_EQ(1, req->xheaders.n);
  EXPECT_STREQ("X-User-Agent", gc(slice(m, req->xheaders.p[0].k)));
  EXPECT_STREQ("hi", gc(slice(m, req->xheaders.p[0].v)));
}

TEST(ParseHttpMessage, testNormalHeaderOnMultipleLines_getsOverwritten) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Content-Type: text/html\r\n\
Content-Type: text/plain\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("text/plain", gc(slice(m, req->headers[kHttpContentType])));
  ASSERT_EQ(0, req->xheaders.n);
}

TEST(ParseHttpMessage, testCommaSeparatedOnMultipleLines_manyLines) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Accept: text/html\r\n\
Accept: text/plain\r\n\
Accept: text/csv\r\n\
Accept: text/xml\r\n\
Accept: text/css\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("text/html", gc(slice(m, req->headers[kHttpAccept])));
  ASSERT_EQ(4, req->xheaders.n);
  EXPECT_STREQ("Accept", gc(slice(m, req->xheaders.p[0].k)));
  EXPECT_STREQ("text/plain", gc(slice(m, req->xheaders.p[0].v)));
  EXPECT_STREQ("Accept", gc(slice(m, req->xheaders.p[1].k)));
  EXPECT_STREQ("text/csv", gc(slice(m, req->xheaders.p[1].v)));
  EXPECT_STREQ("Accept", gc(slice(m, req->xheaders.p[2].k)));
  EXPECT_STREQ("text/xml", gc(slice(m, req->xheaders.p[2].v)));
  EXPECT_STREQ("Accept", gc(slice(m, req->xheaders.p[3].k)));
  EXPECT_STREQ("text/css", gc(slice(m, req->xheaders.p[3].v)));
}

TEST(ParseHttpMessage, testCommaSeparatedOnMultipleLines_becomesLinear) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Accept: text/html\r\n\
Accept: text/plain\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
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
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "gzip", -1));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "deflate", -1));
  EXPECT_FALSE(HeaderHas(req, m, kHttpAcceptEncoding, "funzip", -1));
}

TEST(HeaderHas, testEmptyHeaderName_isNotAllowed) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
: boop\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(-1, ParseHttpMessage(req, m, strlen(m)));
}

TEST(HeaderHas, testHeaderOnSameLIne) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Accept-Encoding: deflate, gzip, bzip2\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "gzip", -1));
  EXPECT_TRUE(HeaderHas(req, m, kHttpAcceptEncoding, "deflate", -1));
  EXPECT_FALSE(HeaderHas(req, m, kHttpAcceptEncoding, "funzip", -1));
}

TEST(ParseHttpMessage, testHeaderValuesWithWhitespace_getsTrimmed) {
  static const char m[] = "\
OPTIONS * HTTP/1.0\r\n\
User-Agent:  \t hi there \t \r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_STREQ("hi there", gc(slice(m, req->headers[kHttpUserAgent])));
  EXPECT_STREQ("*", gc(slice(m, req->uri)));
}

TEST(ParseHttpMessage, testAbsentHost_setsSliceToZero) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(0, req->headers[kHttpHost].a);
  EXPECT_EQ(0, req->headers[kHttpHost].b);
}

TEST(ParseHttpMessage, testEmptyHost_setsSliceToNonzeroValue) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Host:\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_NE(0, req->headers[kHttpHost].a);
  EXPECT_EQ(req->headers[kHttpHost].a, req->headers[kHttpHost].b);
}

TEST(ParseHttpMessage, testEmptyHost2_setsSliceToNonzeroValue) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
Host:    \r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_NE(0, req->headers[kHttpHost].a);
  EXPECT_EQ(req->headers[kHttpHost].a, req->headers[kHttpHost].b);
}

TEST(ParseHttpResponse, testEmpty_tooShort) {
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(0, ParseHttpMessage(req, "", 0));
}

TEST(ParseHttpResponse, testTooShort) {
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(0, ParseHttpMessage(req, "\r\n", 2));
}

TEST(ParseHttpResponse, testNoHeaders) {
  static const char m[] = "HTTP/1.0 200 OK\r\n\r\n";
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(200, req->status);
  EXPECT_STREQ("OK", gc(slice(m, req->message)));
  EXPECT_EQ(10, req->version);
}

TEST(ParseHttpResponse, testSomeHeaders) {
  static const char m[] = "\
HTTP/1.0 200 OK\r\n\
Host: foo.example\r\n\
Content-Length: 0\r\n\
\r\n";
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(200, req->status);
  EXPECT_STREQ("OK", gc(slice(m, req->message)));
  EXPECT_EQ(10, req->version);
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}

TEST(ParseHttpResponse, testHttp101) {
  static const char m[] = "HTTP/1.1 300 OMG\r\n\r\n";
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(300, req->status);
  EXPECT_STREQ("OMG", gc(slice(m, req->message)));
  EXPECT_EQ(11, req->version);
}

TEST(ParseHttpResponse, testHttp100) {
  static const char m[] = "HTTP/1.0 404 Not Found\r\n\r\n";
  InitHttpMessage(req, kHttpResponse);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EXPECT_EQ(404, req->status);
  EXPECT_STREQ("Not Found", gc(slice(m, req->message)));
  EXPECT_EQ(10, req->version);
}

void DoTiniestHttpRequest(void) {
  static const char m[] = "\
GET /\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  ParseHttpMessage(req, m, sizeof(m));
  DestroyHttpMessage(req);
}

void DoTinyHttpRequest(void) {
  static const char m[] = "\
GET /\r\n\
Accept-Encoding: gzip\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  ParseHttpMessage(req, m, sizeof(m));
  DestroyHttpMessage(req);
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
  InitHttpMessage(req, kHttpRequest);
  CHECK_EQ(sizeof(m) - 1, ParseHttpMessage(req, m, sizeof(m)));
  DestroyHttpMessage(req);
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
  InitHttpMessage(req, kHttpRequest);
  CHECK_EQ(sizeof(m) - 1, ParseHttpMessage(req, m, sizeof(m)));
  DestroyHttpMessage(req);
}

void DoTiniestHttpResponse(void) {
  static const char m[] = "\
HTTP/1.0 200\r\n\
\r\n";
  InitHttpMessage(req, kHttpResponse);
  ParseHttpMessage(req, m, sizeof(m));
  DestroyHttpMessage(req);
}

void DoTinyHttpResponse(void) {
  static const char m[] = "\
HTTP/1.0 200\r\n\
Accept-Encoding: gzip\r\n\
\r\n";
  InitHttpMessage(req, kHttpResponse);
  ParseHttpMessage(req, m, sizeof(m));
  DestroyHttpMessage(req);
}

void DoStandardHttpResponse(void) {
  static const char m[] = "\
HTTP/1.1 200 OK\r\n\
Server: nginx\r\n\
Date: Sun, 27 Jun 2021 19:09:59 GMT\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Transfer-Encoding: chunked\r\n\
Connection: keep-alive\r\n\
Vary: Accept-Encoding\r\n\
Cache-Control: private; max-age=0\r\n\
X-Frame-Options: DENY\r\n\
X-Content-Type-Options: nosniff\r\n\
X-XSS-Protection: 1; mode=block\r\n\
Referrer-Policy: origin\r\n\
Strict-Transport-Security: max-age=31556900\r\n\
Content-Security-Policy: default-src 'self'; script-src 'self' 'unsafe-inline' https://www.google.com/recaptcha/ https://www.gstatic.com/recaptcha/ https://cdnjs.cloudflare.com/; frame-src 'self' https://www.google.com/recaptcha/; style-src 'self' 'unsafe-inline'\r\n\
\r\n";
  InitHttpMessage(req, kHttpResponse);
  CHECK_EQ(sizeof(m) - 1, ParseHttpMessage(req, m, sizeof(m)));
  DestroyHttpMessage(req);
}

void DoUnstandardHttpResponse(void) {
  static const char m[] = "\
HTTP/1.1 200 OK\r\n\
date: Sun, 27 Jun 2021 19:00:36 GMT\r\n\
server: Apache\r\n\
x-frame-options: SAMEORIGIN\r\n\
x-xss-protection: 0\r\n\
vary: Accept-Encoding\r\n\
referrer-policy: no-referrer\r\n\
x-slack-backend: r\r\n\
strict-transport-security: max-age=31536000; includeSubDomains; preload\r\n\
set-cookie: b=5aboacm0axrlzntx5wfec7r42; expires=Fri, 27-Jun-2031 19:00:36 GMT; Max-Age=315532800; path=/; domain=.slack.com; secure; SameSite=None\r\n\
set-cookie: x=5aboacm0axrlzntx5wfec7r42.1624820436; expires=Sun, 27-Jun-2021 19:15:36 GMT; Max-Age=900; path=/; domain=.slack.com; secure; SameSite=None\r\n\
content-type: text/html; charset=utf-8\r\n\
x-envoy-upstream-service-time: 19\r\n\
x-backend: main_normal main_canary_with_overflow main_control_with_overflow\r\n\
x-server: slack-www-hhvm-main-iad-a9ic\r\n\
x-via: envoy-www-iad-qd3r, haproxy-edge-pdx-klqo\r\n\
x-slack-shared-secret-outcome: shared-secret\r\n\
via: envoy-www-iad-qd3r\r\n\
transfer-encoding: chunked\r\n\
\r\n";
  InitHttpMessage(req, kHttpResponse);
  CHECK_EQ(sizeof(m) - 1, ParseHttpMessage(req, m, sizeof(m)));
  DestroyHttpMessage(req);
}

BENCH(ParseHttpMessage, bench) {
  EZBENCH2("DoTiniestHttpRequest", donothing, DoTiniestHttpRequest());
  EZBENCH2("DoTinyHttpRequest", donothing, DoTinyHttpRequest());
  EZBENCH2("DoStandardChromeRequest", donothing, DoStandardChromeRequest());
  EZBENCH2("DoUnstandardChromeRequest", donothing, DoUnstandardChromeRequest());
  EZBENCH2("DoTiniestHttpResponse", donothing, DoTiniestHttpResponse());
  EZBENCH2("DoTinyHttpResponse", donothing, DoTinyHttpResponse());
  EZBENCH2("DoStandardHttpResponse", donothing, DoStandardHttpResponse());
  EZBENCH2("DoUnstandardHttpResponse", donothing, DoUnstandardHttpResponse());
}

BENCH(HeaderHas, bench) {
  static const char m[] = "\
GET / HTTP/1.1\r\n\
X-In-Your-Way-A: a\r\n\
X-In-Your-Way-B: b\r\n\
X-In-Your-Way-C: b\r\n\
Accept-Encoding:deflate\r\n\
ACCEPT-ENCODING: gzip\r\n\
ACCEPT-encoding: bzip2\r\n\
\r\n";
  InitHttpMessage(req, kHttpRequest);
  EXPECT_EQ(strlen(m), ParseHttpMessage(req, m, strlen(m)));
  EZBENCH2("HeaderHas text/plain", donothing,
           HeaderHas(req, m, kHttpAccept, "text/plain", 7));
  EZBENCH2("HeaderHas deflate", donothing,
           HeaderHas(req, m, kHttpAcceptEncoding, "deflate", 7));
  EZBENCH2("HeaderHas gzip", donothing,
           HeaderHas(req, m, kHttpAcceptEncoding, "gzip", 4));
}
