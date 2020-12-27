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
#include "libc/bits/safemacros.h"
#include "libc/unicode/unicode.h"
#include "tool/viz/lib/formatstringtable.h"

void *FreeStringTableCells(long yn, long xn, char *T[yn][xn]) {
  long y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      free_s(&T[y][x]);
    }
  }
  return T;
}

static unsigned GetBiggestCellWidth(long yn, long xn,
                                    const char *const T[yn][xn]) {
  long w, y, x;
  for (w = y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      w = max(w, strwidth(T[y][x]));
    }
  }
  return w;
}

void *FormatStringTable(long yn, long xn, const char *const T[yn][xn],
                        int emit(), void *a, const char *startrow,
                        const char *comma, const char *endrow) {
  long w, y, x, i, n;
  w = GetBiggestCellWidth(yn, xn, T);
  for (y = 0; y < yn; ++y) {
    emit(startrow, a);
    for (x = 0; x < xn; ++x) {
      if (x) emit(comma, a);
      for (n = w - strwidth(T[y][x]), i = 0; i < n; ++i) emit(" ", a);
      emit(T[y][x], a);
    }
    emit(endrow, a);
  }
  return (/* unconst */ void *)T;
}
