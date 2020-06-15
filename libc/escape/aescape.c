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
#include "libc/escape/escape.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"

/**
 * Internal function aspecting escaping ops with allocating behavior.
 * @see aescapesh(), aescapec()
 */
int aescape(char **escaped, size_t size, const char *unescaped, unsigned length,
            int impl(char *escaped, unsigned size, const char *unescaped,
                     unsigned length)) {
  int wrote;
  char *p2;
  if ((p2 = realloc(*escaped, size)) != NULL) {
    *escaped = p2;
    if ((wrote = impl(*escaped, size, unescaped, length)) != -1) {
      if ((unsigned)wrote <= size - 1) {
        return wrote;
      } else {
        assert(__builtin_return_address(0) != aescape);
        return aescape(escaped, wrote + 1, unescaped, length, impl);
      }
    }
  }
  free_s(escaped);
  return -1;
}
