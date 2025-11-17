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
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

FILE *f;
char a[16];
char b[16];
char buf[512];

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(ftell, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("hello", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_EQ(0, ftell(f));
  ASSERT_EQ(1, fread(buf, 1, 1, f));
  ASSERT_EQ(1, ftell(f));
  ASSERT_EQ('h', buf[0]);
  ASSERT_EQ(2, fread(buf, 1, 2, f));
  ASSERT_EQ(3, ftell(f));
  ASSERT_EQ('e', buf[0]);
  ASSERT_EQ('l', buf[1]);
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(2, fread(buf, 1, 512, f));
  ASSERT_EQ(5, ftell(f));
  ASSERT_TRUE(feof(f));
  ASSERT_EQ('l', buf[0]);
  ASSERT_EQ('o', buf[1]);
  ASSERT_NE(-1, fclose(f));
}

TEST(ftell, testSmallBuffer) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_NE(-1, setvbuf(f, a, _IOFBF, 1));
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("hello", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_NE(-1, setvbuf(f, b, _IOFBF, 1));
  ASSERT_EQ(0, ftell(f));
  ASSERT_EQ(1, fread(buf, 1, 1, f));
  ASSERT_EQ(1, ftell(f));
  ASSERT_EQ('h', buf[0]);
  ASSERT_EQ(2, fread(buf, 1, 2, f));
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(3, ftell(f));
  ASSERT_EQ('e', buf[0]);
  ASSERT_EQ('l', buf[1]);
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(2, fread(buf, 1, 512, f));
  ASSERT_EQ(5, ftell(f));
  ASSERT_TRUE(feof(f));
  ASSERT_EQ('l', buf[0]);
  ASSERT_EQ('o', buf[1]);
  ASSERT_NE(-1, fclose(f));
}

TEST(ftell, testNoBuffer) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_NE(-1, setvbuf(f, a, _IONBF, 0));
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("hello", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_NE(-1, setvbuf(f, b, _IONBF, 0));
  ASSERT_EQ(0, ftell(f));
  ASSERT_EQ(1, fread(buf, 1, 1, f));
  ASSERT_EQ(1, ftell(f));
  ASSERT_EQ('h', buf[0]);
  ASSERT_EQ(2, fread(buf, 1, 2, f));
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(3, ftell(f));
  ASSERT_EQ('e', buf[0]);
  ASSERT_EQ('l', buf[1]);
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(2, fread(buf, 1, 512, f));
  ASSERT_EQ(5, ftell(f));
  ASSERT_TRUE(feof(f));
  ASSERT_EQ('l', buf[0]);
  ASSERT_EQ('o', buf[1]);
  ASSERT_NE(-1, fclose(f));
}

TEST(fseek, test) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("hello", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_EQ(0, fseek(f, -3, SEEK_CUR));
  ASSERT_EQ(2, ftell(f));
  ASSERT_TRUE(fputs("L", f) >= 0);
  ASSERT_EQ(3, ftell(f));
  ASSERT_EQ(0, fseek(f, -1, SEEK_END));
  ASSERT_EQ(4, ftell(f));
  ASSERT_TRUE(fputs("O", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_EQ(0, fseek(f, 0, SEEK_SET));
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("H", f) >= 0);
  ASSERT_EQ(1, ftell(f));
  ASSERT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_EQ(5, fread(buf, 1, 512, f));
  ASSERT_EQ(0, memcmp("HeLlO", buf, 5));
  ASSERT_NE(-1, fclose(f));
}

TEST(fseek, testMemory) {
  char ss[] = "hello";
  ASSERT_NE(NULL, (f = fmemopen(ss, 5, "r+")));
  ASSERT_EQ(0, fseek(f, 0, SEEK_END));
  ASSERT_EQ(5, ftell(f));
  ASSERT_EQ(0, fseek(f, -3, SEEK_CUR));
  ASSERT_EQ(2, ftell(f));
  ASSERT_TRUE(fputs("L", f) >= 0);
  ASSERT_EQ(3, ftell(f));
  ASSERT_EQ(0, fseek(f, -1, SEEK_END));
  ASSERT_EQ(4, ftell(f));
  ASSERT_TRUE(fputs("O", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_EQ(0, fseek(f, 0, SEEK_SET));
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("H", f) >= 0);
  ASSERT_EQ(1, ftell(f));
  rewind(f);
  ASSERT_EQ(5, fread(buf, 1, 512, f));
  EXPECT_EQ('H', buf[0]);
  EXPECT_EQ('e', buf[1]);
  EXPECT_EQ('L', buf[2]);
  EXPECT_EQ('l', buf[3]);
  EXPECT_EQ('O', buf[4]);
  ASSERT_EQ(0, memcmp("HeLlO", buf, 5));
  ASSERT_NE(-1, fclose(f));
}

TEST(fseek, testSmallBuffer) {
  ASSERT_NE(NULL, (f = fopen("hog", "w")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_NE(-1, setvbuf(f, a, _IOFBF, 1));
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("hello", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_EQ(0, fseek(f, -3, SEEK_CUR));
  ASSERT_EQ(2, ftell(f));
  ASSERT_TRUE(fputs("L", f) >= 0);
  ASSERT_EQ(3, ftell(f));
  ASSERT_EQ(0, fseek(f, -1, SEEK_END));
  ASSERT_EQ(4, ftell(f));
  ASSERT_TRUE(fputs("O", f) >= 0);
  ASSERT_EQ(5, ftell(f));
  ASSERT_EQ(0, fseek(f, 0, SEEK_SET));
  ASSERT_EQ(0, ftell(f));
  ASSERT_TRUE(fputs("H", f) >= 0);
  ASSERT_EQ(1, ftell(f));
  ASSERT_NE(-1, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
#ifdef __FILC__
  errno = 0;
#endif
  ASSERT_EQ(5, fread(buf, 1, 512, f));
  ASSERT_EQ(0, memcmp("HeLlO", buf, 5));
  ASSERT_NE(-1, fclose(f));
}
