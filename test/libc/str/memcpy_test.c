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
#include "libc/bits/bits.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

TEST(memcpy, memcpy) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = tmalloc(n);
    b2 = tmalloc(n);
    ASSERT_EQ(b1, memcpy(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    tfree(b2);
    tfree(b1);
  }
}

TEST(memcpy, memcpyDirect) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = tmalloc(n);
    b2 = tmalloc(n);
    ASSERT_EQ(b1, (memcpy)(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    tfree(b2);
    tfree(b1);
  }
}

TEST(memcpy, mempcpy) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = tmalloc(n);
    b2 = tmalloc(n);
    ASSERT_EQ(b1 + n, mempcpy(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    tfree(b2);
    tfree(b1);
  }
}

TEST(memcpy, mempcpyDirect) {
  char *b1, *b2;
  for (unsigned n = 0; n < 1026; ++n) {
    b1 = tmalloc(n);
    b2 = tmalloc(n);
    ASSERT_EQ(b1 + n, (mempcpy)(b1, b2, n));
    ASSERT_EQ(0, memcmp(b1, b2, n));
    tfree(b2);
    tfree(b1);
  }
}

TEST(memcpy, overlapping_isFineIfCopyingBackwards) {
  for (size_t i = 0; i < 32; ++i) {
    char *b1 = tmalloc(64 + i);
    char *b2 = tmalloc(64 + i);
    memcpy(b1, b2, 64);
    memcpy(b1, b1 + i, 64 - i);
    memmove(b2, b2 + i, 64 - i);
    ASSERT_EQ(0, memcmp(b1, b2, 64));
    tfree(b2);
    tfree(b1);
  }
}

TEST(stpcpy, test) {
  volatile char *p;
  volatile char b[16];
  volatile const char *s1 = "hello";
  volatile const char *s2 = "there";
  p = b;
  p = stpcpy(p, s1);
  EXPECT_EQ((intptr_t)b + 5, (intptr_t)p);
  EXPECT_STREQ("hello", b);
  p = stpcpy(p, s2);
  EXPECT_EQ((intptr_t)b + 10, (intptr_t)p);
  EXPECT_STREQ("hellothere", b);
}

TEST(memcpy, testBackwardsOverlap3) {
  volatile char *c;
  c = malloc(3);
  memcpy(c, "\e[C", 3);
  memcpy(c, c + 1, VEIL("r", 3) - 1);
  EXPECT_EQ('[', c[0]);
  EXPECT_EQ('C', c[1]);
  free(c);
}
