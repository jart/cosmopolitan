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
