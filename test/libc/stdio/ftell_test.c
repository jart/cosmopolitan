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
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

FILE *f;
char buf[512];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(ftell, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(1, fread(buf, 1, 1, f));
  EXPECT_EQ(1, ftell(f));
  EXPECT_EQ('h', buf[0]);
  EXPECT_EQ(2, fread(buf, 1, 2, f));
  EXPECT_EQ(3, ftell(f));
  EXPECT_EQ('e', buf[0]);
  EXPECT_EQ('l', buf[1]);
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(2, fread(buf, 1, 512, f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_TRUE(feof(f));
  EXPECT_EQ('l', buf[0]);
  EXPECT_EQ('o', buf[1]);
  EXPECT_NE(-1, fclose(f));
}

TEST(ftell, testSmallBuffer) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_NE(-1, setvbuf(f, gc(xmalloc(1)), _IOFBF, 1));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_NE(-1, setvbuf(f, gc(xmalloc(1)), _IOFBF, 1));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(1, fread(buf, 1, 1, f));
  EXPECT_EQ(1, ftell(f));
  EXPECT_EQ('h', buf[0]);
  EXPECT_EQ(2, fread(buf, 1, 2, f));
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(3, ftell(f));
  EXPECT_EQ('e', buf[0]);
  EXPECT_EQ('l', buf[1]);
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(2, fread(buf, 1, 512, f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_TRUE(feof(f));
  EXPECT_EQ('l', buf[0]);
  EXPECT_EQ('o', buf[1]);
  EXPECT_NE(-1, fclose(f));
}

TEST(ftell, testNoBuffer) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_NE(-1, setvbuf(f, gc(xmalloc(0)), _IONBF, 0));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_NE(-1, setvbuf(f, gc(xmalloc(0)), _IONBF, 0));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(1, fread(buf, 1, 1, f));
  EXPECT_EQ(1, ftell(f));
  EXPECT_EQ('h', buf[0]);
  EXPECT_EQ(2, fread(buf, 1, 2, f));
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(3, ftell(f));
  EXPECT_EQ('e', buf[0]);
  EXPECT_EQ('l', buf[1]);
  EXPECT_FALSE(feof(f));
  EXPECT_EQ(2, fread(buf, 1, 512, f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_TRUE(feof(f));
  EXPECT_EQ('l', buf[0]);
  EXPECT_EQ('o', buf[1]);
  EXPECT_NE(-1, fclose(f));
}

TEST(fseek, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_EQ(0, fseek(f, -3, SEEK_CUR));
  EXPECT_EQ(2, ftell(f));
  EXPECT_EQ(1, fputs("L", f));
  EXPECT_EQ(3, ftell(f));
  EXPECT_EQ(0, fseek(f, -1, SEEK_END));
  EXPECT_EQ(4, ftell(f));
  EXPECT_EQ(1, fputs("O", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_EQ(0, fseek(f, 0, SEEK_SET));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(1, fputs("H", f));
  EXPECT_EQ(1, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(5, fread(buf, 1, 512, f));
  EXPECT_BINEQ(u"HeLlO", buf);
  EXPECT_NE(-1, fclose(f));
}

TEST(fseek, testMemory) {
  ASSERT_NE(NULL, (f = fmemopen(gc(strdup("hello")), 5, "r+")));
  EXPECT_EQ(0, fseek(f, 0, SEEK_END));
  EXPECT_EQ(5, ftell(f));
  EXPECT_EQ(0, fseek(f, -3, SEEK_CUR));
  EXPECT_EQ(2, ftell(f));
  EXPECT_EQ(1, fputs("L", f));
  EXPECT_EQ(3, ftell(f));
  EXPECT_EQ(0, fseek(f, -1, SEEK_END));
  EXPECT_EQ(4, ftell(f));
  EXPECT_EQ(1, fputs("O", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_EQ(0, fseek(f, 0, SEEK_SET));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(1, fputs("H", f));
  EXPECT_EQ(1, ftell(f));
  rewind(f);
  EXPECT_EQ(5, fread(buf, 1, 512, f));
  EXPECT_BINEQ(u"HeLlO", buf);
  EXPECT_NE(-1, fclose(f));
}

TEST(fseek, testSmallBuffer) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
  EXPECT_NE(-1, setvbuf(f, gc(xmalloc(1)), _IOFBF, 1));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(5, fputs("hello", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_EQ(0, fseek(f, -3, SEEK_CUR));
  EXPECT_EQ(2, ftell(f));
  EXPECT_EQ(1, fputs("L", f));
  EXPECT_EQ(3, ftell(f));
  EXPECT_EQ(0, fseek(f, -1, SEEK_END));
  EXPECT_EQ(4, ftell(f));
  EXPECT_EQ(1, fputs("O", f));
  EXPECT_EQ(5, ftell(f));
  EXPECT_EQ(0, fseek(f, 0, SEEK_SET));
  EXPECT_EQ(0, ftell(f));
  EXPECT_EQ(1, fputs("H", f));
  EXPECT_EQ(1, ftell(f));
  EXPECT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(5, fread(buf, 1, 512, f));
  EXPECT_BINEQ(u"HeLlO", buf);
  EXPECT_NE(-1, fclose(f));
}
