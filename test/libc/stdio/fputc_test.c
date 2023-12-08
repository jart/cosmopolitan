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
#include "libc/runtime/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

FILE *f;
char buf[512];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(fputc, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w+")));
  EXPECT_EQ('h', fputc('h', f));
  EXPECT_EQ(0xFF, fputc(-1, f));
  EXPECT_NE(-1, fseek(f, 0, SEEK_SET));
  EXPECT_EQ('h', fgetc(f));
  EXPECT_EQ(0, fread(NULL, 0, 0, f));
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(0xFF, fgetc(f));
  EXPECT_EQ(-1, fgetc(f));
  EXPECT_TRUE(feof(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(2, fread(buf, 1, 512, f));
  EXPECT_TRUE(!memcmp(buf, "h\377", 2));
  EXPECT_TRUE(feof(f));
  EXPECT_NE(-1, fclose(f));
}

TEST(fgetc, testUnbuffered) {
  ASSERT_NE(NULL, (f = fopen("hog", "w+")));
  EXPECT_NE(-1, setvbuf(f, NULL, _IONBF, 0));
  EXPECT_EQ('h', fputc('h', f));
  EXPECT_EQ(0xFF, fputc(-1, f));
  EXPECT_NE(-1, fseek(f, 0, SEEK_SET));
  EXPECT_EQ('h', fgetc(f));
  EXPECT_EQ(0xFF, fgetc(f));
  EXPECT_EQ(-1, fgetc(f));
  EXPECT_TRUE(feof(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(2, fread(buf, 1, 512, f));
  EXPECT_TRUE(!memcmp(buf, "h\377", 2));
  EXPECT_TRUE(feof(f));
  EXPECT_NE(-1, fclose(f));
}

BENCH(fputc, bench) {
  FILE *f;
  ASSERT_NE(NULL, (f = fopen("/dev/null", "w")));
  EZBENCH2("fputc", donothing, fputc('E', f));
  flockfile(f);
  flockfile(f);
  EZBENCH2("fputc_unlocked", donothing, fputc_unlocked('E', f));
  funlockfile(f);
  funlockfile(f);
  fclose(f);
}
