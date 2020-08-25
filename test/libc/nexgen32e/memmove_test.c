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
