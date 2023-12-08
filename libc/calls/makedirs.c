/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/limits.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates directory and parent components.
 *
 * This function is similar to mkdir() except it iteratively creates
 * parent directories and it won't fail if the directory already exists.
 *
 * @param path is a UTF-8 string, preferably relative w/ forward slashes
 * @param mode can be, for example, 0755
 * @return 0 on success or -1 w/ errno
 * @raise EEXIST if named file already exists as non-directory
 * @raise ENOTDIR if directory component in `path` existed as non-directory
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EROFS if parent directory is on read-only filesystem
 * @raise ENOSPC if file system or parent directory is full
 * @raise EACCES if write permission was denied on parent directory
 * @raise EACCES if search permission was denied on component in `path`
 * @raise ENOENT if `path` is an empty string
 * @raise ELOOP if loop was detected resolving components of `path`
 * @asyncsignalsafe
 */
int makedirs(const char *path, unsigned mode) {
  int c, e, i, n;
  struct stat st;
  char buf[PATH_MAX];

  e = errno;
  n = strlen(path);
  if (n >= PATH_MAX) return enametoolong();
  memcpy(buf, path, n + 1);
  i = n;

  // descend
  while (i) {
    if (!mkdir(buf, mode)) break;
    if (errno == EEXIST) {
      if (i == n) goto CheckTop;
      break;
    }
    if (errno != ENOENT) return -1;
    while (i && buf[i - 1] == '/') buf[--i] = 0;
    while (i && buf[i - 1] != '/') buf[--i] = 0;
  }

  // ascend
  for (;;) {
    if (mkdir(buf, mode)) {
      if (errno != EEXIST) return -1;
      if (i == n) goto CheckTop;
    }
    if (i == n) break;
    while (i < n && (c = path[i]) != '/') buf[i++] = c;
    while (i < n && (c = path[i]) == '/') buf[i++] = c;
  }

Finish:
  errno = e;
  return 0;

CheckTop:
  if (stat(path, &st)) return -1;
  if (S_ISDIR(st.st_mode)) goto Finish;
  return eexist();
}
