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
#include "libc/macros.h"
#include "libc/str/str.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/cga.h"

static const uint8_t kCgaToAnsi[] = {30, 34, 32, 36, 31, 35, 33, 37,
                                     90, 94, 92, 96, 91, 95, 93, 97};

void DrawCga(struct Panel *p, uint8_t v[25][80][2]) {
  unsigned y, x, n, a;
  n = MIN(25, p->bottom - p->top);
  for (y = 0; y < n; ++y) {
    a = -1;
    for (x = 0; x < 80; ++x) {
      if (v[y][x][1] != a) {
        a = v[y][x][1];
        AppendFmt(&p->lines[y], "\e[%d;%dm", kCgaToAnsi[a & 0x0F],
                  kCgaToAnsi[(a & 0xF0) >> 4] + 10);
      }
      AppendWide(&p->lines[y], kCp437[v[y][x][0]]);
    }
    AppendStr(&p->lines[y], "\e[0m");
  }
}
