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
#include "libc/escape/escape.h"
#include "libc/limits.h"
#include "libc/sysv/errfuns.h"

/**
 * Escapes memory for inclusion in C string literals (or Python, etc.)
 *
 * The outer quotation marks are *not* added.
 *
 * @param buf is the output area, which can't overlap and, no matter
 *     what, a NUL-terminator will always be placed in the buffer at
 *     an appropriate place, provided buf!=NULL && size!=0
 * @param size is the byte-length of the output area
 * @param s is the data, which may have NUL characters
 * @param l is the byte-length of s or -1 if s is NUL-terminated
 * @return number of characters written, excluding NUL terminator; or,
 *     if the output buffer wasn't passed, or was too short, then the
 *     number of characters that *would* have been written is returned;
 *     since that's how the snprintf() API works; or -1 on overflow
 * @see xaescapec() for an easier api
 */
int escapec(char *buf, unsigned size, const char *s, unsigned l) {
  if (l >= INT_MAX) return eoverflow();
  unsigned i, j = 0;
  for (i = 0; i < l; ++i) {
    unsigned t = cescapec(s[i]);
    if (t) {
      while (t) {
        if (j < size) {
          buf[j] = (unsigned char)t;
          t >>= 8;
        }
        j++;
      }
    } else {
      if (l == -1) {
        break;
      }
    }
  }
  if (buf && size) {
    if (j < size) {
      buf[j] = '\0';
    } else {
      buf[size - 1] = '\0';
    }
  }
  return j;
}
