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
#include "libc/alg/reverse.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/itoa.h"

noinline size_t uint64toarray(uint64_t i, char *a, int r) {
  size_t j;
  j = 0;
  do {
    a[j++] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i % r];
    i /= r;
  } while (i > 0);
  a[j] = '\0';
  reverse(a, j);
  return j;
}

size_t int64toarray(int64_t i, char *a, int r) {
  if (i < 0) {
    *a++ = '-';
    i = -i;
  }
  return uint64toarray(i, a, r);
}
