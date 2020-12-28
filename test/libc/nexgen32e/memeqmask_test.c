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
#include "libc/runtime/buffer.h"
#include "libc/runtime/gc.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "libc/x/x.h"

#define ALIGN    128
#define BUFSIZE  (8 * 32)
#define MASKSIZE (BUFSIZE / CHAR_BIT)

const char kX[] = "aaaaaaaaeeeeeeeeeeeeeeeeeeeeeeee"
                  "e                              e"
                  "e                              e"
                  "e                              e"
                  "e                              e"
                  "e                              e"
                  "e                              e"
                  "eeeeeeeeeeeeeeeeeeeeeeeeeeeeee-e";

const char kY[] = "aaaaaaaaefffffffffffeffffffffff-"
                  "f                             z-"
                  "f                              f"
                  "f                              f"
                  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                  "f                              f"
                  "f                              f"
                  "ffffffffffffffffffffffffffffff-f";

const char kM[] = "11111111100000000000100000000000"
                  "01111111111111111111111111111100"
                  "01111111111111111111111111111110"
                  "01111111111111111111111111111110"
                  "00000000000000000000000000000000"
                  "01111111111111111111111111111110"
                  "01111111111111111111111111111110"
                  "00000000000000000000000000000010";

nodiscard char *binify(uint8_t *data, size_t size) {
  uint8_t b;
  size_t i, j;
  char *s, *p;
  p = s = xmalloc(size * CHAR_BIT + 1);
  for (i = 0; i < size; ++i) {
    b = data[i];
    for (j = 0; j < CHAR_BIT; ++j) {
      *p++ = "01"[b & 1];
      b >>= 1;
    }
  }
  *p = '\0';
  return s;
}

TEST(memeqmask, test) {
  struct GuardedBuffer x = {}, y = {}, m = {};
  memcpy(balloc(&x, ALIGN, BUFSIZE), kX, BUFSIZE);
  memcpy(balloc(&y, ALIGN, BUFSIZE), kY, BUFSIZE);
  balloc(&m, ALIGN, MASKSIZE);
  EXPECT_EQ((intptr_t)m.p, (intptr_t)memeqmask(m.p, x.p, y.p, BUFSIZE));
  EXPECT_STREQ(kM, gc(binify(m.p, MASKSIZE)));
  bfree(&m);
  bfree(&x);
  bfree(&y);
}

#if 0
#include "libc/rand/rand.h"
#include "libc/testlib/ezbench.h"
TEST(memeqmask, bench) {
  size_t len = 64 * 1024;
  char *m = xmemalign(64, DIMMASK(len));
  char *x = xmemalign(64, len);
  char *y = xmemalign(64, len);
  EZBENCH(
      {
        rngset(x, len, rand64, -1);
        rngset(y, len, rand64, -1);
      },
      memeqmask(m, x, y, len));
}
#endif
