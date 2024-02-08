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
#include "libc/math.h"
#include "libc/mem/gc.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable.h"

static void *ConvertFloatToDoubleMatrix(long yn, long xn, double Md[yn][xn],
                                        const float Mf[yn][xn]) {
  long y, x;
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      Md[y][x] = Mf[y][x];
    }
  }
  return Md;
}

void FormatMatrixFloat(long yn, long xn, const float M[yn][xn], int emit(),
                       void *arg, StringTableFormatter formatter,
                       const char *param1, const char *param2,
                       const char *param3, double digs,
                       double rounder(double)) {
  FormatMatrixDouble(yn, xn,
                     ConvertFloatToDoubleMatrix(
                         yn, xn, gc(xcalloc(yn * xn, sizeof(double))), M),
                     emit, arg, formatter, param1, param2, param3, digs,
                     rounder);
}

char *StringifyMatrixFloat(long yn, long xn, const float M[yn][xn],
                           StringTableFormatter formatter, const char *param1,
                           const char *param2, const char *param3, double digs,
                           double rounder(double)) {
  return StringifyMatrixDouble(
      yn, xn,
      ConvertFloatToDoubleMatrix(yn, xn, gc(xcalloc(yn * xn, sizeof(double))),
                                 M),
      formatter, param1, param2, param3, digs, rounder);
}
