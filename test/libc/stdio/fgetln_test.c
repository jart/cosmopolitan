/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/mem/gc.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"
#include "libc/x/xgetline.h"

TEST(fgetln, test) {
  FILE *f;
  f = fmemopen(gc(strdup(kHyperion)), kHyperionSize, "r+");
  EXPECT_STREQ("The fall of Hyperion - a Dream\n", fgetln(f, 0));
  EXPECT_STREQ("John Keats\n", fgetln(f, 0));
  EXPECT_STREQ("\n", fgetln(f, 0));
  fclose(f);
}

TEST(fgetln, testGoodLastLine) {
  FILE *f;
  size_t n;
  f = fmemopen("foo\nbar\n", 8, "r");
  EXPECT_STREQ("foo\n", fgetln(f, &n));
  EXPECT_EQ(4, n);
  EXPECT_STREQ("bar\n", fgetln(f, &n));
  EXPECT_EQ(4, n);
  EXPECT_EQ(NULL, fgetln(f, 0));
  EXPECT_FALSE(ferror(f));
  EXPECT_TRUE(feof(f));
  fclose(f);
}

TEST(fgetln, testEvilLastLine) {
  FILE *f;
  f = fmemopen("foo\nbar", 7, "r");
  EXPECT_STREQ("foo\n", fgetln(f, 0));
  EXPECT_STREQ("bar", fgetln(f, 0));
  EXPECT_EQ(NULL, fgetln(f, 0));
  EXPECT_FALSE(ferror(f));
  EXPECT_TRUE(feof(f));
  fclose(f);
}

TEST(fgetln, testReadingFromStdin_doesntLeakMemory) {
  int oldstdin, pfds[2];
  oldstdin = dup(0);
  EXPECT_SYS(0, 0, pipe(pfds));
  EXPECT_SYS(0, 8, write(pfds[1], "foo\nbar\n", 8));
  EXPECT_SYS(0, 0, close(pfds[1]));
  EXPECT_SYS(0, 0, close(0));
  EXPECT_SYS(0, 0, dup(pfds[0]));
  EXPECT_SYS(0, 0, close(pfds[0]));
  EXPECT_STREQ("foo\n", fgetln(stdin, 0));
  EXPECT_STREQ("bar\n", fgetln(stdin, 0));
  EXPECT_EQ(NULL, fgetln(stdin, 0));
  EXPECT_FALSE(ferror(stdin));
  EXPECT_TRUE(feof(stdin));
  EXPECT_SYS(0, 0, close(0));
  EXPECT_SYS(0, 0, dup(oldstdin));
  clearerr(stdin);
}

BENCH(fgetln, bench) {
  FILE *f = fmemopen(gc(strdup(kHyperion)), kHyperionSize, "r+");
  EZBENCH2("fgetln", donothing, fgetln(f, 0));
  EZBENCH2("xgetline", donothing, free(xgetline(f)));
  fclose(f);
}
