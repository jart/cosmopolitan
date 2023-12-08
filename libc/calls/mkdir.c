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
#include "libc/calls/calls.h"
#include "libc/sysv/consts/at.h"

/**
 * Creates directory a.k.a. folder.
 *
 *     mkdir o               →  0
 *     mkdir o/yo/yo/yo      → -1 w/ ENOENT
 *     if o/yo is file       → -1 w/ ENOTDIR
 *     if o/yo/yo/yo is dir  → -1 w/ EEXIST
 *     if o/yo/yo/yo is file → -1 w/ EEXIST
 *
 * @param path is a UTF-8 string, preferably relative w/ forward slashes
 * @param mode can be, for example, 0755
 * @return 0 on success or -1 w/ errno
 * @raise EEXIST if named file already exists
 * @raise ENOTDIR if directory component in `path` existed as non-directory
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EROFS if parent directory is on read-only filesystem
 * @raise ENOSPC if file system or parent directory is full
 * @raise EACCES if write permission was denied on parent directory
 * @raise EACCES if search permission was denied on component in `path`
 * @raise ENOENT if a component within `path` didn't exist
 * @raise ENOENT if `path` is an empty string
 * @raise ELOOP if loop was detected resolving components of `path`
 * @see makedirs() which is higher-level
 * @see mkdirat() for modern call
 * @asyncsignalsafe
 */
int mkdir(const char *path, unsigned mode) {
  return mkdirat(AT_FDCWD, path, mode);
}
