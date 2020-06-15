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
#include "libc/conv/conv.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable.h"
#include "tool/viz/lib/stringbuilder.h"

static void *ConvertMatrixToStringTable(long yn, long xn, char *T[yn][xn],
                                        const double M[yn][xn], double digs,
                                        double rounder(double)) {
  double f;
  long y, x;
  assert(yn && xn && !T[0][0]);
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      T[y][x] = xdtoa(RoundDecimalPlaces(M[y][x], digs, rounder));
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
