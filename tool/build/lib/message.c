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
