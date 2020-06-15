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
