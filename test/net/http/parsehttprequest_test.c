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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "net/http/http.h"

TEST(parsehttprequest, testEmpty) {
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  struct HttpRequest *req = calloc(1, sizeof(struct HttpRequest));
  EXPECT_EQ(-1, parsehttprequest(req, f));
  EXPECT_TRUE(feof(f));
  freehttprequest(&req);
  fclose(f);
}

TEST(parsehttprequest, testNoHeaders) {
  const char kMessage[] = "GET /foo HTTP/1.0\r\n"
                          "\r\n";
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(1, fwrite(kMessage, strlen(kMessage), 1, f));
  struct HttpRequest *req = calloc(1, sizeof(struct HttpRequest));
  EXPECT_EQ(0, parsehttprequest(req, f));
  EXPECT_STREQN("GET", req->method.p, req->method.i);
  EXPECT_STREQN("/foo", req->uri.p, req->uri.i);
  EXPECT_STREQN("HTTP/1.0", req->version.p, req->version.i);
  EXPECT_EQ(0, req->headers.count);
  freehttprequest(&req);
  fclose(f);
}

TEST(parsehttprequest, testSomeHeaders) {
  const char kMessage[] = "GET /foo?bar%20hi HTTP/1.0\r\n"
                          "Host: foo.example\r\n"
                          "Content-Length: 0\r\n"
                          "\r\n";
  FILE *f = fmemopen(NULL, BUFSIZ, "r+");
  ASSERT_EQ(1, fwrite(kMessage, strlen(kMessage), 1, f));
  struct HttpRequest *req = calloc(1, sizeof(struct HttpRequest));
  EXPECT_EQ(0, parsehttprequest(req, f));
  EXPECT_STREQN("GET", req->method.p, req->method.i);
  EXPECT_STREQN("/foo?bar%20hi", req->uri.p, req->uri.i);
  EXPECT_STREQN("HTTP/1.0", req->version.p, req->version.i);
  EXPECT_EQ(2, req->headers.count);
  EXPECT_STREQ("host:foo.example", critbit0_get(&req->headers, "host:"));
  EXPECT_STREQ("content-length:0",
               critbit0_get(&req->headers, "content-length:"));
  EXPECT_EQ(NULL, critbit0_get(&req->headers, "content:"));
  freehttprequest(&req);
  fclose(f);
}
