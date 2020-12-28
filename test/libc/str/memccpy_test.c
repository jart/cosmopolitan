/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/ezbench.h"
#include "libc/testlib/testlib.h"

void *memccpy2(void *, const void *, int, size_t);

TEST(memccpy, testStringCopy) {
  char buf[16];
  EXPECT_EQ(buf + 3, memccpy(buf, "hi", '\0', sizeof(buf)));
  EXPECT_STREQ("hi", buf);
}

TEST(memccpy, testOverflow) {
  char buf[1];
  EXPECT_EQ(NULL, memccpy(buf, "hi", '\0', sizeof(buf)));
}

TEST(memccpy, testZeroLength_doesNothing) {
  char buf[1];
  EXPECT_EQ(NULL, memccpy(buf, "hi", '\0', 0));
}

TEST(memccpy, memcpy) {
  unsigned n, n2;
  char *b1, *b2, *b3, *e1, *e2;
  for (n = 0; n < 1026; ++n) {
    b1 = tmalloc(n);
    b2 = tmalloc(n);
    b3 = tmalloc(n);
    e1 = tinymemccpy(b2, b1, 31337, n);
    e2 = memccpy(b3, b1, 31337, n);
    n2 = e1 ? e1 - b1 : n;
    ASSERT_EQ(e1, e2);
    ASSERT_EQ(0, memcmp(b2, b3, n2));
    tfree(b3);
    tfree(b2);
    tfree(b1);
  }
}
