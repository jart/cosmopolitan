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
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/dce.h"
#include "libc/escape/escape.h"
#include "libc/limits.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/errfuns.h"

#define SHQUOTE_PUTCHAR(c)             \
  do {                                 \
    if (buf != NULL && j < size - 1) { \
      buf[j] = (c);                    \
    }                                  \
    j++;                               \
  } while (0)

/**
 * Quotes memory for inclusion in single-quoted SystemV string literals.
 *
 * The outer quotation marks are *not* added.
 *
 * @param buf is the output area, which can't overlap and, no matter
 *     what, a NUL-terminator will always be placed in the buffer at
 *     an appropriate place, provided buf!=NULL && size!=0
 * @param size is the byte-length of the output area
 * @param s is the data, which may have NUL characters
 * @param l is the byte-length of s
 * @return number of characters written, excluding NUL terminator; or,
 *     if the output buffer wasn't passed, or was too short, then the
 *     number of characters that *would* have been written is returned;
 *     since that's how the snprintf() API works; and never < 0
 * @see xaescapesh() for an easier api
 */
int escapesh(char *buf, unsigned size, const char *s, unsigned l) {
  assert(size <= INT_MAX && l <= INT_MAX);
  if (!IsTrustworthy() && l >= INT_MAX) abort();
  unsigned j = 0;
  for (unsigned i = 0; i < l; ++i) {
    if (s[i] != '\'') {
      SHQUOTE_PUTCHAR(s[i]);
    } else {
      const char *const s2 = "'\"'\"'";
      unsigned l2 = 5;
      for (unsigned k = 0; k < l2; ++k) {
        SHQUOTE_PUTCHAR(s2[k]);
      }
    }
  }
  if (buf && size) {
    buf[min(j, size - 1)] = '\0';
  }
  return j;
}
