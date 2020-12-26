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
#include "libc/fmt/fmt.h"
#include "libc/mem/mem.h"

/**
 * Formats string w/ dynamic memory allocation.
 *
 * @param *strp is output-only and must be free'd, even on error; since
 *     that's the behavior that'll make your code most portable
 * @return complete bytes written (excluding NUL) or -1 w/ errno
 * @see xasprintf() for a better API
 */
int(vasprintf)(char **strp, const char *fmt, va_list va) {
  /*
   * This implementation guarantees the smallest possible allocation,
   * using an optimistic approach w/o changing asymptotic complexity.
   */
  size_t size = 32;
  if ((*strp = malloc(size))) {
    va_list va2;
    va_copy(va2, va);
    int wrote = (vsnprintf)(*strp, size, fmt, va);
    if (wrote == -1) return -1;
    if (wrote <= size - 1) {
      return wrote;
    } else {
      size = wrote + 1;
      char *buf2 = realloc(*strp, size);
      if (buf2) {
        *strp = buf2;
        wrote = (vsnprintf)(*strp, size, fmt, va2);
        assert(wrote == size - 1);
        return wrote;
      }
    }
  }
  return -1;
}
