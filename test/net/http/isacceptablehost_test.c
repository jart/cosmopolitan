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
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "net/http/http.h"

TEST(IsAcceptableHost, test) {
  EXPECT_TRUE(IsAcceptableHost("", -1));
  EXPECT_TRUE(IsAcceptableHost("0.0.0.0", -1));
  EXPECT_FALSE(IsAcceptableHost("1.2.3", -1));
  EXPECT_TRUE(IsAcceptableHost("1.2.3.4", -1));
  EXPECT_FALSE(IsAcceptableHost("1.2.3.4.5", -1));
  EXPECT_TRUE(IsAcceptableHost("1.2.3.4.5.arpa", -1));
  EXPECT_TRUE(IsAcceptableHost("255.255.255.255", -1));
  EXPECT_FALSE(IsAcceptableHost("255.255.255", -1));
  EXPECT_TRUE(IsAcceptableHost("hello.example", -1));
  EXPECT_FALSE(IsAcceptableHost("hello..example", -1));
  EXPECT_TRUE(IsAcceptableHost("hello", -1));
  EXPECT_FALSE(IsAcceptableHost("hello\177", -1));
  EXPECT_FALSE(IsAcceptableHost("hello.example\300\200", -1));
  EXPECT_FALSE(IsAcceptableHost(".", -1));
  EXPECT_FALSE(IsAcceptableHost(".e", -1));
  EXPECT_TRUE(IsAcceptableHost("e.", -1));
  EXPECT_FALSE(IsAcceptableHost(".hi.example", -1));
  EXPECT_FALSE(IsAcceptableHost("hi..example", -1));
  EXPECT_TRUE(IsAcceptableHost("hi-there.example", -1));
  EXPECT_TRUE(IsAcceptableHost("_there.example", -1));
  EXPECT_TRUE(IsAcceptableHost("-there.example", -1));
  EXPECT_TRUE(IsAcceptableHost("there-.example", -1));
  EXPECT_FALSE(IsAcceptableHost("ther#e.example", -1));
  EXPECT_TRUE(IsAcceptableHost("localhost", -1));
  EXPECT_TRUE(IsAcceptableHost("8080-gitpodio-empty-abs4xad1abc", -1));
  EXPECT_TRUE(IsAcceptableHost("ws-eu53", -1));
  EXPECT_TRUE(IsAcceptableHost("gitpod", -1));
  EXPECT_TRUE(IsAcceptableHost("io", -1));
}

TEST(IsAcceptablePort, test) {
  EXPECT_TRUE(IsAcceptablePort("", -1));
  EXPECT_TRUE(IsAcceptablePort("0", -1));
  EXPECT_TRUE(IsAcceptablePort("65535", -1));
  EXPECT_FALSE(IsAcceptablePort("65536", -1));
  EXPECT_FALSE(IsAcceptablePort("-1", -1));
  EXPECT_FALSE(IsAcceptablePort("http", -1));
}

BENCH(IsAcceptableHost, bench) {
  EZBENCH2("IsAcceptableHost 127.0.0.1", donothing,
           IsAcceptableHost("127.0.0.1", 9));
  EZBENCH2("IsAcceptablePort 80", donothing, IsAcceptablePort("80", 2));
  EZBENCH2("IsAcceptableHost foo.example", donothing,
           IsAcceptableHost("foo.example:31337", 17));
}
