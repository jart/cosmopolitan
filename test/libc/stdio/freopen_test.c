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
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

/*
 * This test was contributed by @ahgamut
 * https://github.com/jart/cosmopolitan/issues/61#issuecomment-792214575
 */

void SetUpOnce(void) {
  testlib_enable_tmp_setup_teardown();
}

int writefile(const char* filename) {
  int stat = 0;
  FILE* fp = fopen(filename, "w");
  stat = fputs("cosmopolitan libc\n", fp);
  fclose(fp);
  return stat;
}

int readfile(const char* filename) {
  int stat = 0;
  char buf1[30];
  char buf2[30];
  FILE *fp1, *fp2;
  fp1 = fopen(filename, "r");
  if (!fp1) {
    printf("failed to read %s in r\n", filename);
    return 1;
  }
  buf1[0] = fgetc(fp1);
  buf1[1] = '\0';
  fp2 = freopen(filename, "rb", fp1);
  if (!fp2) {
    printf("failed to read %s in rb\n", filename);
    return 1;
  }
  stat = fread(buf2, sizeof(buf2[0]), 20, fp2);
  ASSERT_EQ(18, stat);
  buf2[stat] = '\0';
  fclose(fp2);
  ASSERT_STREQ("c", buf1);
  ASSERT_STREQ("cosmopolitan libc\n", buf2);
  return 0;
}

TEST(freopen, test) {
  writefile("file.txt");
  readfile("file.txt");
}
