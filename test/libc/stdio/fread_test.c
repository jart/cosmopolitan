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
#include "libc/errno.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

TEST(fread, eofIsSticky) {
  FILE *fo, *fi;
  char b[10] = "hello";
  ASSERT_NE(NULL, (fo = fopen("foo", "w")));
  ASSERT_NE(NULL, (fi = fopen("foo", "r")));
  ASSERT_EQ(0, fread(b, 1, 8, fi));
  ASSERT_TRUE(feof(fi));
  ASSERT_EQ(8, fwrite(b, 1, 8, fo));
  ASSERT_EQ(0, fflush(fo));
  ASSERT_EQ(4, fread(b, 1, 4, fi));
  ASSERT_TRUE(feof(fi));
  clearerr(fi);
  ASSERT_EQ(4, fread(b, 1, 10, fi));
  ASSERT_TRUE(feof(fi));
  ASSERT_EQ(0, fseek(fi, 0, SEEK_SET));
  ASSERT_FALSE(feof(fi));
  ASSERT_EQ(0, fclose(fi));
  ASSERT_EQ(0, fclose(fo));
}

TEST(fread, seekWithBuffer) {
  FILE* f;
  char b[8] = "hellosup";
  char c[8] = {0};
  char d[8] = {0};
  ASSERT_NE(NULL, (f = fopen("foo", "w")));
  ASSERT_EQ(8, fwrite(b, 1, 8, f));
  ASSERT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("foo", "r")));
  ASSERT_EQ(5, fread(c, 1, 5, f));
  ASSERT_STREQ("hello", c);
  ASSERT_EQ(0, fseek(f, 1, SEEK_SET));
  ASSERT_EQ(5, fread(d, 1, 5, f));
  ASSERT_STREQ("ellos", d);
  ASSERT_EQ(0, fclose(f));
}

TEST(fread, zero) {
  FILE* f;
  char buf[8] = {0};
  ASSERT_NE(NULL, (f = fopen("foo", "w")));
  ASSERT_EQ(2, fwrite("hi", 1, 2, f));
  ASSERT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("foo", "r")));
  ASSERT_EQ(0, fread(buf, 0, 0, f));
  ASSERT_EQ(0, ferror(stdin));
  ASSERT_EQ(0, feof(stdin));
  ASSERT_STREQ("", buf);
  ASSERT_EQ(0, fclose(f));
}

TEST(fread, partial) {
  FILE* f;
  char buf[8] = {0};
  ASSERT_NE(NULL, (f = fopen("foo", "w")));
  ASSERT_EQ(2, fwrite("hi", 1, 2, f));
  ASSERT_EQ(0, fclose(f));
  ASSERT_NE(NULL, (f = fopen("foo", "r")));
  ASSERT_EQ(0, fread(buf, 8, 1, f));
  ASSERT_EQ(0, ferror(stdin));
  ASSERT_EQ(0, feof(stdin));
  ASSERT_EQ(0, fclose(f));
  ASSERT_STREQ("hi", buf);
}
