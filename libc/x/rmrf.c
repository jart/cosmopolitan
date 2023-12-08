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
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/stdio/ftw.h"
#include "libc/sysv/errfuns.h"
#include "libc/x/x.h"

static inline bool IsSlash(char c) {
  return c == '/' || c == '\\';
}

static inline int IsAlpha(int c) {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

static int rmrf_callback(const char *fpath,      //
                         const struct stat *st,  //
                         int typeflag,           //
                         struct FTW *ftwbuf) {   //
  int rc;
  if (typeflag == FTW_DNR) {
    if (!(rc = chmod(fpath, 0700))) {
      return nftw(fpath, rmrf_callback, 128 - ftwbuf->level,
                  FTW_PHYS | FTW_DEPTH);
    }
  } else if (typeflag == FTW_DP) {
    rc = rmdir(fpath);
  } else {
    rc = unlink(fpath);
  }
  if (rc == -1 && errno == ENOENT) {
    rc = 0;
  }
  return rc;
}

/**
 * Recursively removes file or directory.
 * @return 0 on success, or -1 w/ errno
 */
int rmrf(const char *path) {
  if ((IsSlash(path[0]) && !path[1]) ||
      (IsWindows() && ((IsSlash(path[0]) && IsAlpha(path[1]) &&
                        (!path[2] || (IsSlash(path[2]) && !path[3]))) ||
                       (IsAlpha(path[0]) && path[1] == ':' &&
                        (!path[2] || (IsSlash(path[2]) && !path[3])))))) {
    return enotsup();  // if you really want rmrf("/") try rmrf("/.")
  }
  return nftw(path, rmrf_callback, 128, FTW_PHYS | FTW_DEPTH);
}
