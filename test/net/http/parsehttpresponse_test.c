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
#include "libc/log/check.h"
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

void SetUp(void) {
  InitHttpResponse(req);
}

void TearDown(void) {
  DestroyHttpResponse(req);
}

TEST(ParseHttpResponse, soLittleState) {
  ASSERT_LE(sizeof(struct HttpMessage), 512);
}

TEST(ParseHttpResponse, testEmpty_tooShort) {
  EXPECT_EQ(0, ParseHttpResponse(req, "", 0));
}

TEST(ParseHttpResponse, testTooShort) {
  EXPECT_EQ(0, ParseHttpResponse(req, "\r\n", 2));
}

TEST(ParseHttpResponse, testNoHeaders) {
  static const char m[] = "HTTP/1.0 200 OK\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpResponse(req, m, strlen(m)));
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
  EXPECT_EQ(strlen(m), ParseHttpResponse(req, m, strlen(m)));
  EXPECT_EQ(200, req->status);
  EXPECT_STREQ("OK", gc(slice(m, req->message)));
  EXPECT_EQ(10, req->version);
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}

TEST(ParseHttpResponse, testHttp101) {
  static const char m[] = "HTTP/1.1 300 OMG\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpResponse(req, m, strlen(m)));
  EXPECT_EQ(300, req->status);
  EXPECT_STREQ("OMG", gc(slice(m, req->message)));
  EXPECT_EQ(11, req->version);
}

TEST(ParseHttpResponse, testHttp100) {
  static const char m[] = "HTTP/1.0 404 Not Found\r\n\r\n";
  EXPECT_EQ(strlen(m), ParseHttpResponse(req, m, strlen(m)));
  EXPECT_EQ(404, req->status);
  EXPECT_STREQ("Not Found", gc(slice(m, req->message)));
  EXPECT_EQ(10, req->version);
}

void DoTiniestHttpResponse(void) {
  static const char m[] = "\
HTTP/1.0 200\r\n\
\r\n";
  InitHttpResponse(req);
  ParseHttpResponse(req, m, sizeof(m));
  DestroyHttpResponse(req);
}

void DoTinyHttpResponse(void) {
  static const char m[] = "\
HTTP/1.0 200\r\n\
Accept-Encoding: gzip\r\n\
\r\n";
  InitHttpResponse(req);
  ParseHttpResponse(req, m, sizeof(m));
  DestroyHttpResponse(req);
}

void DoStandardChromeResponse(void) {
  static const char m[] = "\
HTTP/1.1 200 OK\r\n\
Host: 10.10.10.124:8080\r\n\
Connection: keep-alive\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36\r\n\
DNT:  \t1   \r\n\
Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n\
Referer: http://10.10.10.124:8080/\r\n\
Accept-Encoding: gzip, deflate\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
\r\n";
  InitHttpResponse(req);
  CHECK_EQ(sizeof(m) - 1, ParseHttpResponse(req, m, sizeof(m)));
  DestroyHttpResponse(req);
}

void DoUnstandardChromeResponse(void) {
  static const char m[] = "\
HTTP/1.1 200 OK\r\n\
X-Host: 10.10.10.124:8080\r\n\
X-Connection: keep-alive\r\n\
X-User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/89.0.4389.90 Safari/537.36\r\n\
X-DNT:  \t1   \r\n\
X-Accept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\n\
X-Referer: http://10.10.10.124:8080/\r\n\
X-Accept-Encoding: gzip, deflate\r\n\
X-Accept-Language: en-US,en;q=0.9\r\n\
\r\n";
  InitHttpResponse(req);
  CHECK_EQ(sizeof(m) - 1, ParseHttpResponse(req, m, sizeof(m)));
  DestroyHttpResponse(req);
}

BENCH(ParseHttpResponse, bench) {
  EZBENCH2("DoTiniestHttpResponse", donothing, DoTiniestHttpResponse());
  EZBENCH2("DoTinyHttpResponse", donothing, DoTinyHttpResponse());
  EZBENCH2("DoStandardChromeResponse", donothing, DoStandardChromeResponse());
  EZBENCH2("DoUnstandardChromeResponse", donothing,
           DoUnstandardChromeResponse());
}

BENCH(HeaderHas, bench) {
  static const char m[] = "\
HTTP/1.1 200 OK\r\n\
X-In-Your-Way-A: a\r\n\
X-In-Your-Way-B: b\r\n\
X-In-Your-Way-C: b\r\n\
Accept-Encoding: deflate\r\n\
ACCEPT-ENCODING: gzip\r\n\
ACCEPT-encoding: bzip2\r\n\
\r\n";
  EXPECT_EQ(strlen(m), ParseHttpResponse(req, m, strlen(m)));
  EZBENCH2("HeaderHas text/plain", donothing,
           HeaderHas(req, m, kHttpAccept, "text/plain", 7));
  EZBENCH2("HeaderHas deflate", donothing,
           HeaderHas(req, m, kHttpAcceptEncoding, "deflate", 7));
  EZBENCH2("HeaderHas gzip", donothing,
           HeaderHas(req, m, kHttpAcceptEncoding, "gzip", 4));
  EZBENCH2("IsMimeType", donothing,
           IsMimeType("text/plain; charset=utf-8", -1, "text/plain"));
}
