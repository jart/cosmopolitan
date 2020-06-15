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
#include "libc/math.h"
#include "libc/runtime/gc.h"
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
