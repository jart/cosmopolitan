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
#include "dsp/tty/quant.h"
#include "dsp/tty/xtermname.h"
#include "libc/conv/conv.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/ex.h"
#include "third_party/dtoa/dtoa.h"

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
           tty.xt, tty.xt, indexdoublenulstring(kXtermName, tty.xt), r, g, b);
  }
  return 0;
}
