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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

TEST(fgets, test) {
  FILE *f;
  char buf[29];
  f = fmemopen(gc(strdup(kHyperion)), kHyperionSize, "r+");
  ASSERT_STREQ("The fall of Hyperion - a Dre", fgets(buf, sizeof(buf), f));
  ASSERT_STREQ("am\n", fgets(buf, sizeof(buf), f));
  ASSERT_STREQ("John Keats\n", fgets(buf, sizeof(buf), f));
  fclose(f);
}

TEST(fgets, eof) {
  FILE *f;
  char s[10];
  char buf[] = "test";
  ASSERT_NE(NULL, (f = fmemopen(buf, sizeof buf, "r")));
  ASSERT_EQ(s, fgets(s, sizeof s, f));
  ASSERT_EQ(0, strcmp(s, buf));
  ASSERT_EQ(0, fgets(s, sizeof s, f));
  ASSERT_EQ('t', s[0]);
  ASSERT_EQ(0, fclose(f));
}

void Benchmark(void) {
  FILE *f;
  char *line;
  char buf[512];
  f = fmemopen(gc(strdup(kHyperion)), kHyperionSize, "r+");
  for (;;) {
    line = fgets(buf, sizeof(buf), f);
    if (!line) break;
  }
  fclose(f);
}

BENCH(fgets, bench) {
  EZBENCH2("fgets", donothing, Benchmark());
}
