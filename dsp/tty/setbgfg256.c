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
