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
#include "libc/testlib/testlib.h"
#include "net/http/http.h"

TEST(IsAcceptableHostPort, test) {
  EXPECT_FALSE(IsAcceptableHostPort("", -1));
  EXPECT_FALSE(IsAcceptableHostPort(":", -1));
  EXPECT_FALSE(IsAcceptableHostPort(":80", -1));
  EXPECT_TRUE(IsAcceptableHostPort("0.0.0.0", -1));
  EXPECT_FALSE(IsAcceptableHostPort("1.2.3", -1));
  EXPECT_TRUE(IsAcceptableHostPort("1.2.3.4", -1));
  EXPECT_FALSE(IsAcceptableHostPort("1.2.3.4.5", -1));
  EXPECT_TRUE(IsAcceptableHostPort("1.2.3.4.5.arpa", -1));
  EXPECT_TRUE(IsAcceptableHostPort("255.255.255.255", -1));
  EXPECT_FALSE(IsAcceptableHostPort("255.255.255", -1));
  EXPECT_FALSE(IsAcceptableHostPort("256.255.255.255", -1));
  EXPECT_TRUE(IsAcceptableHostPort("hello.example", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello..example", -1));
  EXPECT_TRUE(IsAcceptableHostPort("hello.example:80", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example:80:", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example::80", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example:-80", -1));
  EXPECT_FALSE(IsAcceptableHostPort(":80", -1));
  EXPECT_TRUE(IsAcceptableHostPort("hello.example:65535", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example:65536", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example:-80", -1));
  EXPECT_FALSE(IsAcceptableHostPort(" hello .example:80", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example:80h", -1));
  EXPECT_TRUE(IsAcceptableHostPort("hello", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello\177", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hello.example\300\200:80", -1));
  EXPECT_FALSE(IsAcceptableHostPort(".", -1));
  EXPECT_FALSE(IsAcceptableHostPort(".e", -1));
  EXPECT_FALSE(IsAcceptableHostPort("e.", -1));
  EXPECT_FALSE(IsAcceptableHostPort(".hi.example", -1));
  EXPECT_FALSE(IsAcceptableHostPort("hi..example", -1));
  EXPECT_TRUE(IsAcceptableHostPort("hi-there.example", -1));
  EXPECT_TRUE(IsAcceptableHostPort("_there.example", -1));
  EXPECT_TRUE(IsAcceptableHostPort("-there.example", -1));
  EXPECT_TRUE(IsAcceptableHostPort("there-.example", -1));
  EXPECT_FALSE(IsAcceptableHostPort("ther#e.example", -1));
}
