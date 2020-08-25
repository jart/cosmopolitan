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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/testlib/testlib.h"

TEST(memset, testMulTrick4) {
  long i, j;
  unsigned long x;
  long di, si, dx, ax;
  volatile uint8_t *b;
  b = tgc(tmalloc(4));
  for (i = 0; i < 255; ++i) {
    for (j = -1; j < 1; ++j) {
      x = j;
      x &= ~0xff;
      x |= i;
      asm volatile("call\tmemset"
                   : "=D"(di), "=S"(si), "=d"(dx), "=a"(ax)
                   : "0"(b), "1"(x), "2"(4)
                   : "rcx", "memory", "cc");
      ASSERT_EQ(x & 0xff, b[0]);
      ASSERT_EQ(x & 0xff, b[1]);
      ASSERT_EQ(x & 0xff, b[2]);
      ASSERT_EQ(x & 0xff, b[3]);
    }
  }
}

TEST(memset, testMulTrick8) {
  long i, j;
  unsigned long x;
  long di, si, dx, ax;
  volatile uint8_t *b;
  b = tgc(tmalloc(8));
  for (i = 0; i < 255; ++i) {
    for (j = -1; j < 1; ++j) {
      x = j;
      x &= ~0xff;
      x |= i;
      asm volatile("call\tmemset"
                   : "=D"(di), "=S"(si), "=d"(dx), "=a"(ax)
                   : "0"(b), "1"(x), "2"(8)
                   : "rcx", "memory", "cc");
      ASSERT_EQ(x & 0xff, b[0]);
      ASSERT_EQ(x & 0xff, b[1]);
      ASSERT_EQ(x & 0xff, b[2]);
      ASSERT_EQ(x & 0xff, b[3]);
      ASSERT_EQ(x & 0xff, b[4]);
      ASSERT_EQ(x & 0xff, b[5]);
      ASSERT_EQ(x & 0xff, b[6]);
      ASSERT_EQ(x & 0xff, b[7]);
    }
  }
}
