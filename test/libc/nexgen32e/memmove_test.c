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
#include "libc/bits/safemacros.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

#define N 256
#define S 7

long i, j, n;
char *b1, *b2;

TEST(memmove, overlapping) {
  for (i = 0; i < N; i += S) {
    for (j = 10; j < N; j += S) {
      b1 = tmalloc(N);
      b2 = tmalloc(N);
      n = min(N - i, N - j);
      memcpy(b2, b1 + i, n);
      ASSERT_EQ(b1 + j, memmove(b1 + j, b1 + i, n));
      ASSERT_EQ(0, memcmp(b1 + j, b2, n));
      tfree(b2);
      tfree(b1);
    }
  }
}

TEST(memmove, overlappingDirect) {
  for (i = 0; i < N; i += S) {
    for (j = 10; j < N; j += S) {
      b1 = tmalloc(N);
      b2 = tmalloc(N);
      n = min(N - i, N - j);
      memcpy(b2, b1 + i, n);
      ASSERT_EQ(b1 + j, (memmove)(b1 + j, b1 + i, n));
      ASSERT_EQ(0, memcmp(b1 + j, b2, n));
      tfree(b2);
      tfree(b1);
    }
  }
}

char *MoveMemory(char *dst, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    dst[i] = src[i];
  }
  return dst;
}

TEST(memmove, overlappingBackwards_isGenerallySafe) {
  char buf[32];
  strcpy(buf, "abcdefghijklmnopqrstuvwxyz");
  ASSERT_STREQ("cdefghijklmnopqrstuvwxyzyz", MoveMemory(buf, buf + 2, 24));
  strcpy(buf, "abcdefghijklmnopqrstuvwxyz");
  ASSERT_STREQ("cdefghijklmnopqrstuvwxyzyz", memmove(buf, buf + 2, 24));
  strcpy(buf, "abcdefghijklmnopqrstuvwxyz");
  ASSERT_STREQ("cdefghijklmnopqrstuvwxyzyz", memcpy(buf, buf + 2, 24));
}

TEST(memmove, overlappingForwards_avoidsRunLengthDecodeBehavior) {
  volatile char buf[32];
  strcpy(buf, "abc");
  MoveMemory(buf + 1, buf, 2);
  ASSERT_STREQ("aaa", buf);
  strcpy(buf, "abc");
  (memmove)(buf + 1, buf, 2);
  ASSERT_STREQ("aab", buf);
  strcpy(buf, "abcdefghijklmnopqrstuvwxyz");
  MoveMemory(buf + 2, buf, 24);
  ASSERT_STREQ("ababababababababababababab", buf);
  strcpy(buf, "abcdefghijklmnopqrstuvwxyz");
  memmove(buf + 2, buf, 24);
  ASSERT_STREQ("ababcdefghijklmnopqrstuvwx", buf);
}
