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
#include "libc/log/log.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/sysv/consts/fileno.h"

/**
 * Shorthand for getting ws_col from getttysize().
 *
 * It is recommended that programs err on the side of showing more
 * information, if this value can't be obtained with certainty.
 *
 * @return terminal width or defaultcols on error
 */
uint16_t getttycols(uint16_t defaultcols) {
  struct winsize wsize;
  if (getttysize(STDIN_FILENO, &wsize) != -1) {
    return wsize.ws_col;
  } else {
    return defaultcols;
  }
}
