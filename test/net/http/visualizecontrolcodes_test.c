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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xasprintf.h"
#include "net/http/escape.h"

TEST(VisualizeControlCodes, test) {
  size_t n;
  EXPECT_STREQ("hello", gc(VisualizeControlCodes("hello", -1, 0)));
  EXPECT_STREQ("hello\r\n", gc(VisualizeControlCodes("hello\r\n", -1, 0)));
  EXPECT_STREQ("hello␁␂␡", gc(VisualizeControlCodes("hello\1\2\177", -1, 0)));
  EXPECT_STREQ("hello\\u0085",
               gc(VisualizeControlCodes("hello\302\205", -1, 0)));
  EXPECT_STREQ("hi", gc(VisualizeControlCodes("hi", -1, &n)));
  EXPECT_EQ(2, n);
}

TEST(VisualizeControlCodes, testOom_returnsNullAndSetsSizeToZero) {
  size_t n = 31337;
  EXPECT_EQ(NULL, gc(VisualizeControlCodes("hello", 0x1000000000000, &n)));
  EXPECT_EQ(0, n);
}

TEST(VisualizeControlCodes, testWeirdHttp) {
  size_t n = 31337;
  char *p, B[] = "\0GET /redbean.lua\n\n";
  ASSERT_NE(0, (p = gc(VisualizeControlCodes(B, sizeof(B), &n))));
  EXPECT_STREQ("\"␀GET /redbean.lua\\n\\n␀\"", gc(xasprintf("%`'.*s", n, p)));
}

BENCH(VisualizeControlCodes, bench) {
  EZBENCH2("VisualizeControlCodes", donothing,
           free(VisualizeControlCodes(kHyperion, kHyperionSize, 0)));
}
