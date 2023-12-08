/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/fmt/libgen.h"
#include "libc/str/str.h"

/**
 * Returns pointer to last filename component in path, e.g.
 *
 *     path     │ dirname() │ basename()
 *     ─────────────────────────────────
 *     0        │ .         │ .
 *     .        │ .         │ .
 *     ..       │ .         │ ..
 *     /        │ /         │ /
 *     usr      │ .         │ usr
 *     /usr/    │ /         │ usr
 *     /usr/lib │ /usr      │ lib
 *
 * Both / and \ are are considered valid component separators on all
 * platforms. Trailing slashes are ignored. We don't grant special
 * consideration to things like foo/., c:/, \\?\Volume, etc.
 *
 * @param path is UTF-8 and may be mutated, but not expanded in length
 * @return pointer to path, or inside path, or to a special r/o string
 * @see dirname()
 * @see SUSv2
 */
char *basename(char *path) {
  size_t i;
  if (path && *path) {
    i = strlen(path) - 1;
    for (; i && path[i] == '/'; i--) {
      path[i] = 0;
    }
    while (i && path[i - 1] != '/') {
      i--;
    }
    return path + i;
  } else {
    return ".";
  }
}
