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
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/http.h"

struct HttpRequest req[1];

static char *slice(const char *m, struct HttpRequestSlice s) {
  return memcpy(xmalloc(s.b - s.a), m + s.a, s.b - s.a);
}

TEST(ParseHttpRequest, testEmpty) {
  EXPECT_EQ(-1, ParseHttpRequest(req, "", 0));
}

TEST(ParseHttpRequest, testNoHeaders) {
  static const char m[] = "GET /foo HTTP/1.0\r\n\r\n";
  EXPECT_EQ(0, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpGet, req->method);
  EXPECT_STREQ("/foo", gc(slice(m, req->uri)));
  EXPECT_STREQ("HTTP/1.0", gc(slice(m, req->version)));
}

TEST(ParseHttpRequest, testSomeHeaders) {
  static const char m[] = "\
POST /foo?bar%20hi HTTP/1.0\r\n\
Host: foo.example\r\n\
Content-Length: 0\r\n\
\r\n";
  EXPECT_EQ(0, ParseHttpRequest(req, m, strlen(m)));
  EXPECT_EQ(kHttpPost, req->method);
  EXPECT_STREQ("/foo?bar%20hi", gc(slice(m, req->uri)));
  EXPECT_STREQ("HTTP/1.0", gc(slice(m, req->version)));
  EXPECT_STREQ("foo.example", gc(slice(m, req->headers[kHttpHost])));
  EXPECT_STREQ("0", gc(slice(m, req->headers[kHttpContentLength])));
  EXPECT_STREQ("", gc(slice(m, req->headers[kHttpEtag])));
}
