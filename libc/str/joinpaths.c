/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/str/str.h"

/**
 * Joins paths, e.g.
 *
 *     0    + 0    → 0
 *     ""   + ""   → ""
 *     "a"  + 0    → "a"
 *     "a"  + ""   → "a/"
 *     0    + "b"  → "b"
 *     ""   + "b"  → "b"
 *     "."  + "b"  → "./b"
 *     "b"  + "."  → "b/."
 *     "a"  + "b"  → "a/b"
 *     "a/" + "b"  → "a/b"
 *     "a"  + "b/" → "a/b/"
 *     "a"  + "/b" → "/b"
 *
 * @return joined path, which may be `buf`, `path`, or `other`, or null
 *     if (1) `buf` didn't have enough space, or (2) both `path` and
 *     `other` were null
 */
char *__join_paths(char *buf, size_t size, const char *path,
                   const char *other) {
  size_t pathlen, otherlen;
  if (!other) return (char *)path;
  if (!path) return (char *)other;
  pathlen = strlen(path);
  if (!pathlen || *other == '/') {
    return (/*unconst*/ char *)other;
  }
  otherlen = strlen(other);
  if (path[pathlen - 1] == '/') {
    if (pathlen + otherlen + 1 <= size) {
      memmove(buf, path, pathlen);
      memmove(buf + pathlen, other, otherlen + 1);
      return buf;
    } else {
      return 0;
    }
  } else {
    if (pathlen + 1 + otherlen + 1 <= size) {
      memmove(buf, path, pathlen);
      buf[pathlen] = '/';
      memmove(buf + pathlen + 1, other, otherlen + 1);
      return buf;
    } else {
      return 0;
    }
  }
}
