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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/hyperion.h"
#include "libc/testlib/testlib.h"

FILE *f;
char buf[512];
char testlib_enable_tmp_setup_teardown;

TEST(ungetc, testGetChar_canBeUndoneWithinReason) {
  ASSERT_NE(NULL, (f = fopen("hog", "wb")));
  EXPECT_EQ(12, fputs("hello world\n", f));
  EXPECT_NE(-1, fputs(kHyperion, f));
  EXPECT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ('h', fgetc(f));
  EXPECT_EQ('e', fgetc(f));
  EXPECT_EQ('e', ungetc('e', f));
  EXPECT_EQ('h', ungetc('h', f));
  EXPECT_EQ(5, fread(buf, 1, 5, f));
  EXPECT_BINEQ(u"hello", buf);
  EXPECT_EQ(0, fclose(f));
}

TEST(ungetc, testRead_canBeUndoneWithinReason) {
  ASSERT_NE(NULL, (f = fopen("hog", "wb")));
  EXPECT_EQ(12, fputs("hello world\n", f));
  EXPECT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(3, fread(buf, 1, 3, f));
  EXPECT_BINEQ(u"hel", buf);
  EXPECT_EQ('l', ungetc(buf[2], f));
  EXPECT_EQ('e', ungetc(buf[1], f));
  EXPECT_EQ(4, fread(buf, 1, 4, f));
  EXPECT_BINEQ(u"ello", buf);
  EXPECT_EQ(0, fclose(f));
}

TEST(ungetwc, testGetWideChar_canBeUndoneWithinReason) {
  memset(buf, 0, sizeof(buf));
  ASSERT_NE(NULL, (f = fopen("hog", "wb")));
  EXPECT_NE(-1, fputs("𐌰𐌱\n", f));
  EXPECT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("hog", "r")));
  EXPECT_EQ(L'𐌰', fgetwc(f));
  EXPECT_EQ(L'𐌱', fgetwc(f));
  EXPECT_EQ(L'𐌱', ungetwc(L'𐌱', f));
  EXPECT_EQ(L'𐌰', ungetwc(L'𐌰', f));
  EXPECT_NE(0, fread(buf, 1, sizeof(buf), f));
  EXPECT_STREQ("𐌰𐌱\n", buf);
  EXPECT_EQ(0, fclose(f));
}
