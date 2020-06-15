/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/stdio/stdio.h"
#include "libc/calls/calls.h"
#include "libc/sysv/consts/o.h"

/**
 * Turns stdio flags description string into bitmask.
 */
int fopenflags(const char *mode) {
  unsigned flags = 0;
  do {
    if (*mode == 'r') {
      flags |= O_RDONLY;
    } else if (*mode == 'w') {
      flags |= O_WRONLY | O_CREAT | O_TRUNC;
    } else if (*mode == 'a') {
      flags |= O_WRONLY | O_CREAT | O_APPEND;
    } else if (*mode == '+') {
      flags |= O_RDWR;
    } else if (*mode == 'x') {
      flags |= O_EXCL;
    } else if (*mode == 'e') {
      flags |= O_CLOEXEC;
    }
  } while (*mode++);
  return flags;
}
