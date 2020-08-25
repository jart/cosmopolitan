/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
