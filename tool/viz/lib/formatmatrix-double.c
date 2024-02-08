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
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/mem/mem.h"
#include "libc/x/x.h"
#include "third_party/gdtoa/gdtoa.h"
#include "tool/viz/lib/formatstringtable.h"
#include "tool/viz/lib/stringbuilder.h"

void *ConvertMatrixToStringTable(long yn, long xn, char *T[yn][xn],
                                 const double M[yn][xn], double digs,
                                 double rounder(double)) {
  long y, x;
  assert(yn && xn && !T[0][0]);
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      T[y][x] = xmalloc(40);
      T[y][x][0] = '\0';
      g_dfmt_p(T[y][x], (void *)&M[y][x], digs, 40, 0);
    }
  }
  return T;
}

void FormatMatrixDouble(long yn, long xn, const double M[yn][xn], int emit(),
                        void *arg, StringTableFormatter formatter,
                        const char *param1, const char *param2,
                        const char *param3, double digs,
                        double rounder(double)) {
  FreeStringTableCells(
      yn, xn,
      formatter(
          yn, xn,
          ConvertMatrixToStringTable(
              yn, xn, gc(calloc(yn * xn, sizeof(char *))), M, digs, rounder),
          emit, arg, param1, param2, param3));
}

char *StringifyMatrixDouble(long yn, long xn, const double M[yn][xn],
                            StringTableFormatter formatter, const char *param1,
                            const char *param2, const char *param3, double digs,
                            double rounder(double)) {
  struct StringBuilder *sb = NewStringBuilder();
  FormatMatrixDouble(yn, xn, M, StringBuilderAppend, sb, formatter, param1,
                     param2, param3, digs, rounder);
  return FreeStringBuilder(sb);
}
