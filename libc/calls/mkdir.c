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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/nt/files.h"
#include "libc/nt/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"

static textwindows noinline int mkdir$nt(const char *path, uint32_t mode) {
  uint16_t path16[PATH_MAX];
  if (__mkntpath(path, path16) == -1) return -1;
  if (CreateDirectory(path16, NULL)) {
    return 0;
  } else {
    return __winerr();
  }
}

/**
 * Creates directory a.k.a. folder.
 *
 * @param path is a UTF-8 string, preferably relative w/ forward slashes
 * @param mode can be, for example, 0755
 * @return 0 on success or -1 w/ errno
 * @error EEXIST, ENOTDIR, ENAMETOOLONG, EACCES
 * @asyncsignalsafe
 */
int mkdir(const char *path, uint32_t mode) {
  if (!path) return efault();
  if (!IsWindows()) {
    return mkdirat$sysv(AT_FDCWD, path, mode);
  } else {
    return mkdir$nt(path, mode);
  }
}
