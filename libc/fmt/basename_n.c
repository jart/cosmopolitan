/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/conv.h"
#include "libc/fmt/isslash.internal.h"

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
      while (l && isslash(path[l - 1])) --l;
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
