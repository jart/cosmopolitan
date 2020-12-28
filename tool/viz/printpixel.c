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
#include "dsp/tty/quant.h"
#include "dsp/tty/xtermname.h"
#include "libc/fmt/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"

int main(int argc, char *argv[]) {
  int i;
  struct TtyRgb tty;
  unsigned rgb, r, g, b;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s RRGGBB...\n", program_invocation_name);
    exit(EX_USAGE);
  }
  for (i = 1; i < argc; ++i) {
    rgb = strtol(argv[i], NULL, 16);
    b = (rgb & 0x0000ff) >> 000;
    g = (rgb & 0x00ff00) >> 010;
    r = (rgb & 0xff0000) >> 020;
    tty = rgb2tty(r, g, b);
    printf("\e[48;5;%dm    \e[0m %d \\e[48;5;%dm %s #%02x%02x%02x\n", tty.xt,
           tty.xt, tty.xt, IndexDoubleNulString(kXtermName, tty.xt), r, g, b);
  }
  return 0;
}
