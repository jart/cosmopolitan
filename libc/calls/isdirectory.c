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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/sysdebug.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/nt/files.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Returns true if file exists and is a directory.
 *
 * This function is equivalent to:
 *
 *     struct stat st;
 *     return fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW) != -1 &&
 *            S_ISDIR(st.st_mode);
 *
 * Except faster and with fewer dependencies.
 *
 * @see isregularfile(), issymlink(), ischardev()
 */
bool isdirectory(const char *path) {
  int e;
  bool res;
  union metastat st;
  struct ZiposUri zipname;
  e = errno;
  if (IsAsan() && !__asan_is_valid(path, 1)) return efault();
  if (weaken(__zipos_open) && weaken(__zipos_parseuri)(path, &zipname) != -1) {
    if (weaken(__zipos_stat)(&zipname, &st.cosmo) != -1) {
      res = S_ISDIR(st.cosmo.st_mode);
    } else {
      res = false;
    }
  } else if (IsMetal()) {
    res = false;
  } else if (!IsWindows()) {
    e = errno;
    if (__sys_fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW) != -1) {
      res = S_ISDIR(METASTAT(st, st_mode));
    } else {
      res = false;
    }
  } else {
    res = isdirectory_nt(path);
  }
  SYSDEBUG("isdirectory(%s) -> %s %s", path, res ? "true" : "false",
           res ? "" : strerror(errno));
  if (!res && (errno == ENOENT || errno == ENOTDIR)) {
    errno = e;
  }
  return res;
}
