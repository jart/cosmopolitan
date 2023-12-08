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
#include "tool/viz/lib/formatstringtable.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/strwidth.h"

void *FreeStringTableCells(long yn, long xn, char *T[yn][xn]) {
  long y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      free(T[y][x]);
      T[y][x] = 0;
    }
  }
  return T;
}

static unsigned GetBiggestCellWidth(long yn, long xn,
                                    const char *const T[yn][xn]) {
  long w, y, x;
  for (w = y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      w = max(w, strwidth(T[y][x], 0));
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
      for (n = w - strwidth(T[y][x], 0), i = 0; i < n; ++i) emit(" ", a);
      emit(T[y][x], a);
    }
    emit(endrow, a);
  }
  return (/* unconst */ void *)T;
}
