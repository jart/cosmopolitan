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
#include "libc/str/str.h"

static char *rgbcpy(char *p, struct TtyRgb bg) {
  memcpy(p, ";2;\0", 4);
  p = itoa8(p + 3, bg.r);
  *p++ = ';';
  p = itoa8(p, bg.g);
  *p++ = ';';
  return itoa8(p, bg.b);
}

char *setbg24_(char *p, struct TtyRgb bg) {
  memcpy(p, "\e[48", 4);
  p = rgbcpy(p + 4, bg);
  *p++ = 'm';
  return p;
}

char *setfg24_(char *p, struct TtyRgb fg) {
  memcpy(p, "\e[38", 4);
  p = rgbcpy(p + 4, fg);
  *p++ = 'm';
  return p;
}

char *setbgfg24_(char *p, struct TtyRgb bg, struct TtyRgb fg) {
  memcpy(p, "\e[48", 4);
  p = rgbcpy(p + 4, bg);
  memcpy(p, ";38\0", 4);
  p = rgbcpy(p + 3, fg);
  *p++ = 'm';
  return p;
}
