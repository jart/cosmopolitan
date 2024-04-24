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
#include "libc/calls/calls.h"
#include "libc/log/check.h"
#include "libc/mem/mem.h"
#include "libc/mem/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"
#include "net/http/escape.h"

char *o;
size_t y;

void TearDownOnce(void) {
  free(o);
  o = 0;
  y = 0;
}

char *escapejs(const char *s) {
  char *p;
  size_t n;
  p = EscapeJsStringLiteral(&o, &y, s, strlen(s), &n);
  ASSERT_EQ(strlen(p), n);
  return p;
}

TEST(EscapeJsStringLiteral, test) {
  EXPECT_STREQ("", escapejs(""));
  EXPECT_STREQ("\\u00ff", escapejs("\377"));
  EXPECT_STREQ("\\u00ff\\u0080\\u0080\\u0080\\u0080",
               escapejs("\377\200\200\200\200"));
  EXPECT_STREQ("\\u0001\\u0002\\u0003 \\u0026\\u003d\\u003c\\u003e\\/",
               escapejs("\1\2\3 &=<>/"));
}

TEST(EscapeJsStringLiteral, testUcs2) {
  EXPECT_STREQ("\\u00d0\\u263b", escapejs("Ð☻"));
}

TEST(EscapeJsStringLiteral, testAstralPlanes) {
  EXPECT_STREQ("\\ud800\\udf30\\ud800\\udf30", escapejs("𐌰𐌰"));
}

TEST(EscapeJsStringLiteral, testBrokenUnicode_sparesInnocentCharacters) {
  EXPECT_STREQ("\\u00e1YO", escapejs("\xE1YO"));
}

void makefile1(void) {
  FILE *f;
  char *p;
  size_t n;
  p = EscapeJsStringLiteral(&o, &y, kHyperion, kHyperionSize, &n);
  f = tmpfile();
  fwrite(p, n, 1, f);
  fclose(f);
}

void makefile2(void) {
  int fd;
  char *p;
  size_t n;
  p = EscapeJsStringLiteral(&o, &y, kHyperion, kHyperionSize, &n);
  fd = tmpfd();
  write(fd, p, n);
  close(fd);
}

BENCH(EscapeJsStringLiteral, bench) {
  EZBENCH2("escapejs tiny", donothing,
           EscapeJsStringLiteral(&o, &y, "hello", 5, 0));
  EZBENCH2("escapejs book", donothing,
           EscapeJsStringLiteral(&o, &y, kHyperion, kHyperionSize, 0));
  EZBENCH2("makefile1", donothing, makefile1());
  EZBENCH2("makefile2", donothing, makefile2());
}
