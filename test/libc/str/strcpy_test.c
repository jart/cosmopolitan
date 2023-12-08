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
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

TEST(strcpy, test) {
  char buf[64];
  EXPECT_STREQ("hello", strcpy(buf, "hello"));
  EXPECT_STREQ("hello there what's up", strcpy(buf, "hello there what's up"));
}

TEST(strncpy, test) {
  char b[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  strncpy(b, "key", 8);
  ASSERT_EQ('k', b[0]);
  ASSERT_EQ('e', b[1]);
  ASSERT_EQ('y', b[2]);
  ASSERT_EQ('\0', b[3]);
  ASSERT_EQ('\0', b[4]);
  ASSERT_EQ('\0', b[5]);
  ASSERT_EQ('\0', b[6]);
  ASSERT_EQ('\0', b[7]);
}

TEST(strncpy, testSameLength_isNotNulTerminated) {
  char b[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  strncpy(b, "keyword", 7);
  ASSERT_EQ('k', b[0]);
  ASSERT_EQ('e', b[1]);
  ASSERT_EQ('y', b[2]);
  ASSERT_EQ('w', b[3]);
  ASSERT_EQ('o', b[4]);
  ASSERT_EQ('r', b[5]);
  ASSERT_EQ('d', b[6]);
  ASSERT_EQ(7, b[7]);
}

BENCH(strcpy, bench) {
  extern char *strcpy_(char *, const char *) asm("strcpy");
  static char buf[1024], buf2[1024];
  memset(buf2, -1, sizeof(buf2) - 1);
  EZBENCH2("strcpy 1", donothing, strcpy_(buf, ""));
  EZBENCH2("strcpy 2", donothing, strcpy_(buf, "1"));
  EZBENCH2("strcpy 7", donothing, strcpy_(buf, "123456"));
  EZBENCH2("strcpy 8", donothing, strcpy_(buf, "1234567"));
  EZBENCH2("strcpy 9", donothing, strcpy_(buf, "12345678"));
  EZBENCH2("strcpy 16", donothing, strcpy_(buf, "123456781234567"));
  EZBENCH2("strcpy 1023", donothing, strcpy_(buf, buf2));
}
