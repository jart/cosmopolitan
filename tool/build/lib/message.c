/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/log/check.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "tool/build/lib/buffer.h"
#include "tool/build/lib/lines.h"
#include "tool/build/lib/panel.h"

static int GetWidthOfLongestLine(struct Lines *lines) {
  int i, w, m;
  for (m = i = 0; i < lines->n; ++i) {
    w = strwidth(lines->p[i], 0);
    m = MAX(m, w);
  }
  return m;
}

void PrintMessageBox(int fd, const char *msg, long tyn, long txn) {
  struct Buffer b;
  int i, w, h, x, y;
  struct Lines *lines;
  lines = NewLines();
  AppendLines(lines, msg);
  h = 3 + lines->n + 3;
  w = 4 + GetWidthOfLongestLine(lines) + 4;
  x = (txn / 2. - w / 2.) + .5;
  y = (tyn / 2. - h / 2.) + .5;
  bzero(&b, sizeof(b));
  AppendFmt(&b, "\e[%d;%dH", y++, x);
  for (i = 0; i < w; ++i) AppendStr(&b, " ");
  AppendFmt(&b, "\e[%d;%dH ╔", y++, x);
  for (i = 0; i < w - 4; ++i) AppendStr(&b, "═");
  AppendStr(&b, "╗ ");
  AppendFmt(&b, "\e[%d;%dH ║  %-*s  ║ ", y++, x, w - 8, "");
  for (i = 0; i < lines->n; ++i) {
    AppendFmt(&b, "\e[%d;%dH ║  %-*s  ║ ", y++, x, w - 8, lines->p[i]);
  }
  FreeLines(lines);
  AppendFmt(&b, "\e[%d;%dH ║  %-*s  ║ ", y++, x, w - 8, "");
  AppendFmt(&b, "\e[%d;%dH ╚", y++, x);
  for (i = 0; i < w - 4; ++i) AppendStr(&b, "═");
  AppendStr(&b, "╝ ");
  AppendFmt(&b, "\e[%d;%dH", y++, x);
  for (i = 0; i < w; ++i) AppendStr(&b, " ");
  CHECK_NE(-1, WriteBuffer(&b, fd));
  free(b.p);
}
