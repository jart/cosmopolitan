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
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "net/http/escape.h"

TEST(IndentLines, testEmpty) {
  char *p;
  size_t n;
  static const char kInput[] = "";
  static const char kOutput[] = "";
  p = gc(IndentLines(kInput, -1, &n, 2));
  EXPECT_STREQ(kOutput, p);
  EXPECT_EQ(strlen(kOutput), n);
}

TEST(IndentLines, test) {
  char *p;
  size_t n;
  static const char kInput[] = "\
HTTP/1.1 405 Method Not Allowed\r\n\
Content-Type: text/plain; charset=utf-8\r\n\
Date: Sun, 28 Mar 2021 10:47:47 GMT\r\n\
Server: redbean/0.2\r\n\
Content-Length: 20\r\n\
\r\n";
  static const char kOutput[] = "\
  HTTP/1.1 405 Method Not Allowed\r\n\
  Content-Type: text/plain; charset=utf-8\r\n\
  Date: Sun, 28 Mar 2021 10:47:47 GMT\r\n\
  Server: redbean/0.2\r\n\
  Content-Length: 20\r\n\
\r\n";
  p = gc(IndentLines(kInput, -1, &n, 2));
  EXPECT_STREQ(kOutput, p);
  EXPECT_EQ(strlen(kOutput), n);
}

BENCH(IndentLines, bench) {
  EZBENCH2("IndentLines", donothing,
           free(IndentLines(kHyperion, kHyperionSize, 0, 2)));
}
