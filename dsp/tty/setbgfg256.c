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
#include "dsp/tty/itoa8.h"
#include "dsp/tty/quant.h"

char *setbg256_(char *p, struct TtyRgb bg) {
  memcpy(p, "\e[48", 4);
  memcpy(p + 4, ";5;\0", 4);
  p = itoa8(p + 7, bg.xt);
  *p++ = 'm';
  return p;
}

char *setfg256_(char *p, struct TtyRgb fg) {
  memcpy(p, "\e[38", 4);
  memcpy(p + 4, ";5;\0", 4);
  p = itoa8(p + 7, fg.xt);
  *p++ = 'm';
  return p;
}

char *setbgfg256_(char *p, struct TtyRgb bg, struct TtyRgb fg) {
  memcpy(p, "\e[48", 4);
  memcpy(p + 4, ";5;\0", 4);
  p = itoa8(p + 7, bg.xt);
  memcpy(p, ";38;", 4);
  memcpy(p + 4, "5;\0", 4);
  p = itoa8(p + 6, fg.xt);
  *p++ = 'm';
  return p;
}
