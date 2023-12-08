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
#include "libc/limits.h"

static char *ansitoa(char *p, unsigned xt, unsigned base) {
  if (xt >= 8) xt -= 8, base += 60;
  return itoa8(p, xt + base);
}

static char *setansibgfg(char *p, unsigned bg, unsigned fg) {
  *p++ = '\e';
  *p++ = '[';
  if (bg != -1u) p = ansitoa(p, bg, 40);
  if (bg != -1u && fg != -1u) *p++ = ';';
  if (fg != -1u) p = ansitoa(p, fg, 30);
  *p++ = 'm';
  return p;
}

char *setbg16_(char *p, struct TtyRgb bg) { return setansibgfg(p, bg.xt, -1u); }
char *setfg16_(char *p, struct TtyRgb fg) { return setansibgfg(p, -1u, fg.xt); }
char *setbgfg16_(char *p, struct TtyRgb bg, struct TtyRgb fg) {
  return setansibgfg(p, bg.xt, fg.xt);
}
