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
#include "libc/bits/safemacros.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/fmt/conv.h"
#include "libc/log/log.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/termios.h"

/**
 * Returns dimensions of controlling terminal.
 *
 * It is recommended that programs err on the side of showing more
 * information, if this value can't be obtained with certainty.
 *
 * @param out stores determined dimensions, only on success
 * @returns -1 on error or something else on success
 */
int getttysize(int fd, struct winsize *out) {
  if (isterminalinarticulate()) {
    out->ws_col = strtoimax(firstnonnull(getenv("COLUMNS"), "80"), NULL, 0);
    out->ws_row = strtoimax(firstnonnull(getenv("ROWS"), "40"), NULL, 0);
    out->ws_xpixel = 0;
    out->ws_ypixel = 0;
    return 0;
  } else {
    return ioctl(fd, TIOCGWINSZ, out);
  }
}
