/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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

FILE *f;
char buf[512];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(fputs, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(5, fread(buf, 1, 512, f));
  EXPECT_TRUE(!memcmp(buf, "hello", 5));
  EXPECT_TRUE(feof(f));
  EXPECT_NE(-1, fclose(f));
}

TEST(puts, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_EQ('\n', fputc('\n', f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(6, fread(buf, 1, 512, f));
  EXPECT_TRUE(!memcmp(buf, "hello\n", 6));
  EXPECT_TRUE(feof(f));
  EXPECT_NE(-1, fclose(f));
}

BENCH(fputs, bench) {
  char *buf = gc(malloc(kHyperionSize));
  EZBENCH2("fputs", f = fmemopen(buf, kHyperionSize, "r+"),
           fputs(kHyperion, f));
}
