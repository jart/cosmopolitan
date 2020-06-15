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
