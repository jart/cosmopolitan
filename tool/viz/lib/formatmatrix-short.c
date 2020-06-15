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
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/runtime/gc.h"
#include "libc/x/x.h"
#include "tool/viz/lib/formatstringtable.h"
#include "tool/viz/lib/stringbuilder.h"

static void *ConvertShortMatrixToStringTable(long yn, long xn, char *T[yn][xn],
                                             const short M[yn][xn]) {
  long y, x;
  assert(yn && xn && !T[0][0]);
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      T[y][x] = xasprintf("%d", M[y][x]);
    }
  }
  return T;
}

void FormatMatrixShort(long yn, long xn, const short M[yn][xn], int emit(),
                       void *arg, StringTableFormatter formatter,
                       const char *param1, const char *param2,
                       const char *param3) {
  FreeStringTableCells(
      yn, xn,
      formatter(yn, xn,
                ConvertShortMatrixToStringTable(
                    yn, xn, gc(xcalloc(yn * xn, sizeof(char *))), M),
                emit, arg, param1, param2, param3));
}

char *StringifyMatrixShort(long yn, long xn, const short M[yn][xn],
                           StringTableFormatter formatter, const char *param1,
                           const char *param2, const char *param3) {
  struct StringBuilder *sb = NewStringBuilder();
  FormatMatrixShort(yn, xn, M, StringBuilderAppend, sb, formatter, param1,
                    param2, param3);
  return FreeStringBuilder(sb);
}
