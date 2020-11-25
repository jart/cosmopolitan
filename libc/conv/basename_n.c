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
#include "libc/conv/conv.h"
#include "libc/conv/isslash.internal.h"

/**
 * Returns pointer to last filename component in path.
 *
 * Both / and \ are are considered valid component separators on all
 * platforms. Trailing slashes are ignored. We don't grant special
 * consideration to things like foo/., c:/, \\?\Volume, etc.
 *
 * @param path is UTF-8 path
 * @param size is byte length of path
 * @return pointer inside path or path itself
 */
textstartup char *basename_n(const char *path, size_t size) {
  size_t i, l;
  if (size) {
    if (isslash(path[size - 1])) {
      l = size - 1;
      while (isslash(path[l - 1])) --l;
      if (!l) return (/*unconst*/ char *)&path[size - 1];
      size = l;
    }
    for (i = size; i > 0; --i) {
      if (isslash(path[i - 1])) {
        return (/*unconst*/ char *)&path[i];
      }
    }
  }
  return (/*unconst*/ char *)path;
}
