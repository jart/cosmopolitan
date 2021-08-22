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
#include "libc/mem/mem.h"
#include "libc/rand/rand.h"
#include "libc/runtime/gc.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/madv.h"
#include "libc/sysv/consts/o.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define N (72 * 1024)

char p[N];
char testlib_enable_tmp_setup_teardown;

TEST(filecmp, testEqual) {
  rngset(p, N, rand64, -1);
  EXPECT_EQ(0, xbarf("a", p, N));
  EXPECT_EQ(0, xbarf("b", p, N));
  EXPECT_EQ(0, filecmp("a", "b"));
}

TEST(filecmp, testNotEqual) {
  rngset(p, N, rand64, -1);
  EXPECT_EQ(0, xbarf("a", p, N));
  p[N / 2]++;
  EXPECT_EQ(0, xbarf("b", p, N));
  EXPECT_NE(0, filecmp("a", "b"));
}

TEST(filecmp, testDifferentLength) {
  rngset(p, N, rand64, -1);
  EXPECT_EQ(0, xbarf("a", p, N));
  EXPECT_EQ(0, xbarf("b", p, N - 1));
  EXPECT_NE(0, filecmp("a", "b"));
}

BENCH(filecmp, bench) {
  rngset(p, N, rand64, -1);
  EXPECT_EQ(0, xbarf("a", p, N));
  EXPECT_EQ(0, xbarf("b", p, N));
  EZBENCH2("filecmp", donothing, filecmp("a", "b"));
}
