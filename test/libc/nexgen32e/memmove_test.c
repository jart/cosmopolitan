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
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"

#define N 65
#define S 1

long i, j, n;
char *b0, *b1, *b2;

dontinline char *PosixMemmove(char *dst, const char *src, size_t n) {
  char *tmp;
  tmp = malloc(n);
  memcpy(tmp, src, n);
  memcpy(dst, tmp, n);
  free(tmp);
  return dst;
}

TEST(MemMove, overlapping) {
  for (i = 0; i < N; i += S) {
    for (j = 0; j < N; j += S) {
      for (n = MIN(N - i, N - j) + 1; n--;) {
        b0 = rngset(malloc(N), N, _rand64, -1);
        b1 = memcpy(malloc(N), b0, N);
        b2 = memcpy(malloc(N), b0, N);
        ASSERT_EQ(b1 + j, memmove(b1 + j, b1 + i, n));
        ASSERT_EQ(b2 + j, PosixMemmove(b2 + j, b2 + i, n));
        ASSERT_EQ(0, memcmp(b1, b2, N),
                  "j=%ld i=%ld n=%ld\n"
                  "\t%#.*s data\n"
                  "\t%#.*s memmove\n"
                  "\t%#.*s posix",
                  j, i, n, n, b0, n, b1, n, b2);
        free(b2);
        free(b1);
        free(b0);
      }
    }
  }
}

TEST(MemCpy, overlapping) {
  for (i = 0; i < N; i += S) {
    for (j = 0; j < N; j += S) {
      for (n = MIN(N - i, N - j) + 1; n--;) {
        if (j <= i) {
          b0 = rngset(malloc(N), N, _rand64, -1);
          b1 = memcpy(malloc(N), b0, N);
          b2 = memcpy(malloc(N), b0, N);
          ASSERT_EQ(b1 + j, memcpy(b1 + j, b1 + i, n));
          ASSERT_EQ(b2 + j, PosixMemmove(b2 + j, b2 + i, n));
          ASSERT_EQ(0, memcmp(b1, b2, N),
                    "j=%ld i=%ld n=%ld\n"
                    "\t%#.*s data\n"
                    "\t%#.*s memmove\n"
                    "\t%#.*s posix",
                    j, i, n, n, b0, n, b1, n, b2);
          free(b2);
          free(b1);
          free(b0);
        }
      }
    }
  }
}

TEST(MemMove, overlappingDirect) {
  for (i = 0; i < N; i += S) {
    for (j = 0; j < N; j += S) {
      for (n = MIN(N - i, N - j) + 1; n--;) {
        b0 = rngset(malloc(N), N, _rand64, -1);
        b1 = memcpy(malloc(N), b0, N);
        b2 = memcpy(malloc(N), b0, N);
        ASSERT_EQ(b1 + j, (memmove)(b1 + j, b1 + i, n));
        ASSERT_EQ(b2 + j, PosixMemmove(b2 + j, b2 + i, n));
        ASSERT_EQ(0, memcmp(b1, b2, N),
                  "j=%ld i=%ld n=%ld\n"
                  "\t%#.*s data\n"
                  "\t%#.*s memmove\n"
                  "\t%#.*s posix",
                  j, i, n, n, b0, n, b1, n, b2);
        free(b2);
        free(b1);
        free(b0);
      }
    }
  }
}
