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
#include "dsp/tty/tty.h"
#include "libc/bits/pushpop.h"
#include "libc/dce.h"
#include "libc/log/log.h"
#include "libc/nt/console.h"
#include "libc/nt/runtime.h"
#include "libc/nt/struct/consolecursorinfo.h"

static int ttysetcursor(int fd, bool visible) {
  struct NtConsoleCursorInfo ntcursor;
  char code[8] = "\e[?25l";
  if (isterminalinarticulate()) return 0;
  if (visible) code[5] = 'h';
  if (SupportsWindows()) {
    GetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
    ntcursor.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(kNtStdOutputHandle), &ntcursor);
  }
  return ttysend(fd, code);
}

/**
 * Asks teletypewriter to hide blinking box.
 */
int ttyhidecursor(int fd) {
  return ttysetcursor(fd, false);
}

/**
 * Asks teletypewriter to restore blinking box.
 */
int ttyshowcursor(int fd) {
  return ttysetcursor(fd, true);
}
