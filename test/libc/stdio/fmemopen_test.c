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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/stdio/stdio.h"
#include "libc/temp.h"
#include "libc/testlib/testlib.h"

TEST(fmemopen, testWriteRewindRead) {
  char c;
  FILE *f;
  f = fmemopen(NULL, BUFSIZ, "w+");
  EXPECT_EQ(1, fwrite("c", 1, 1, f));
  rewind(f);
  EXPECT_EQ(1, fread(&c, 1, 1, f));
  EXPECT_EQ('c', c);
  fclose(f);
}

TEST(fmemopen_fprintf, test) {
  FILE *f = fmemopen(NULL, BUFSIZ, "w+");
  EXPECT_EQ(1, fprintf(f, "%ld", 0L));
  rewind(f);
  char buf[8] = {0};
  EXPECT_EQ(1, fread(&buf, 1, 8, f));
  EXPECT_STREQ("0", buf);
  fclose(f);
}

TEST(fmemopen, seekEofRead) {
  FILE *f = fmemopen("x", 1, "r+");
  ASSERT_SYS(EINVAL, -1, fseek(f, -1, SEEK_SET));
  ASSERT_SYS(EINVAL, -1, fseek(f, +1, SEEK_END));
  ASSERT_EQ(0, fseek(f, 0, SEEK_END));
  ASSERT_FALSE(feof(f));
  ASSERT_EQ(-1, fgetc(f));
  ASSERT_TRUE(feof(f));
  fclose(f);
}

TEST(tmpfile_fprintf, test) {
  FILE *f = tmpfile();
  EXPECT_EQ(1, fprintf(f, "%ld", 0L));
  rewind(f);
  char buf[8] = {0};
  EXPECT_EQ(1, fread(&buf, 1, 8, f));
  EXPECT_STREQ("0", buf);
  fclose(f);
}

TEST(fmemopen, small) {
  FILE *f = fmemopen(gc(malloc(1)), 1, "w+");
  EXPECT_EQ(3, fprintf(f, "%ld", 123L));
  rewind(f);
  char buf[8] = {0};
  EXPECT_EQ(1, fread(&buf, 1, 8, f));
  EXPECT_STREQ("1", buf);
  fclose(f);
}
