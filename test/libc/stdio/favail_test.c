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
#include "libc/assert.h"
#include "libc/bits/popcnt.h"
#include "libc/stdio/stdio.h"
#include "libc/testlib/testlib.h"

unsigned naive(unsigned beg, unsigned end, unsigned size) {
  assert(end < size);
  assert(beg < size);
  assert(popcnt(size) == 1);
  if (beg == end) return size;
  if (end > beg) return end - beg;
  return (size - beg) + end;
}

unsigned fancy(unsigned beg, unsigned end, unsigned size) {
  return ((end - beg - 1) & (size - 1)) + 1;
}

TEST(favail, test) {
  unsigned i, j, n = 4;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      ASSERT_EQ(naive(i, j, n), fancy(i, j, n), "%u %u %u", i, j, n);
    }
  }
}
