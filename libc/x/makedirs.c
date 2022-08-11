/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/safemacros.internal.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/strace.internal.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/x/x.h"

static int MakeDirs(const char *path, unsigned mode, int e) {
  int rc;
  char *dir;
  if (!mkdir(path, mode) || errno == EEXIST) {
    errno = e;
    return 0;
  }
  if (errno != ENOENT) {
    return -1;
  }
  dir = xdirname(path);
  if (strcmp(dir, path)) {
    rc = MakeDirs(dir, mode, e);
  } else {
    rc = -1;
  }
  free(dir);
  if (rc == -1) return -1;
  errno = e;
  return mkdir(path, mode);
}

/**
 * Recursively creates directory a.k.a. folder.
 *
 * This function won't fail if the directory already exists.
 *
 * @param path is a UTF-8 string, preferably relative w/ forward slashes
 * @param mode can be, for example, 0755
 * @return 0 on success or -1 w/ errno
 * @see mkdir()
 */
int makedirs(const char *path, unsigned mode) {
  return MakeDirs(path, mode, errno);
}
