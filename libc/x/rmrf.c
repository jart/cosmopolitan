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
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/struct/dirent.h"
#include "libc/calls/struct/stat.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/stdio.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/dt.h"
#include "libc/sysv/consts/s.h"
#include "libc/x/x.h"

static int rmrfdir(const char *dirpath) {
  int rc;
  DIR *d;
  char *path;
  struct dirent *e;
  if (!(d = opendir(dirpath))) return -1;
  while ((e = readdir(d))) {
    if (!strcmp(e->d_name, ".")) continue;
    if (!strcmp(e->d_name, "..")) continue;
    _npassert(!strchr(e->d_name, '/'));
    path = xjoinpaths(dirpath, e->d_name);
    if (e->d_type == DT_DIR) {
      rc = rmrfdir(path);
    } else {
      rc = unlink(path);
    }
    free(path);
    if (rc == -1) {
      closedir(d);
      return -1;
    }
  }
  rc = closedir(d);
  rc |= rmdir(dirpath);
  return rc;
}

/**
 * Recursively removes file or directory.
 *
 * @return 0 on success, or -1 w/ errno
 */
int rmrf(const char *path) {
  int e;
  struct stat st;
  e = errno;
  if (stat(path, &st) == -1) {
    if (errno == ENOENT) {
      errno = e;
      return 0;
    } else {
      return -1;
    }
  }
  if (!S_ISDIR(st.st_mode)) {
    return unlink(path);
  } else {
    return rmrfdir(path);
  }
}
