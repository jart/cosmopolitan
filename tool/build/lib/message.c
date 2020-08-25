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
#include "libc/log/check.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/panel.h"

void PrintMessageBox(int fd, const char *msg, long tyn, long txn) {
  char buf[1];
  struct Buffer b;
  int i, w, h, x, y;
  h = 2 + 1 + 2;
  w = 3 + strlen(msg) + 3;
  x = lrint(txn / 2. - w / 2.);
  y = lrint(tyn / 2. - h / 2.);
  memset(&b, 0, sizeof(b));
  AppendFmt(&b, "\e[%d;%dH╔", y++, x);
  for (i = 0; i < w - 2; ++i) AppendStr(&b, "═");
  AppendStr(&b, "╗");
  AppendFmt(&b, "\e[%d;%dH║  %-*s  ║", y++, x, w - 6, "");
  AppendFmt(&b, "\e[%d;%dH║  %-*s  ║", y++, x, w - 6, msg);
  AppendFmt(&b, "\e[%d;%dH║  %-*s  ║", y++, x, w - 6, "");
  AppendFmt(&b, "\e[%d;%dH╚", y++, x);
  for (i = 0; i < w - 2; ++i) AppendStr(&b, "═");
  AppendStr(&b, "╝");
  CHECK_NE(-1, WriteBuffer(&b, fd));
  free(b.p);
}
