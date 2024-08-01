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
#include "libc/calls/struct/metastat.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/nt/files.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns true if file exists and is a symbolic link.
 *
 * This function is equivalent to:
 *
 *     struct stat st;
 *     return fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW) != -1 &&
 *            S_ISLNK(st.st_mode);
 *
 * Except faster and with fewer dependencies.
 *
 * @see isregularfile(), isdirectory(), fileexists(), ischardev()
 */
bool32 issymlink(const char *path) {
  int e;
  bool res;
  union metastat st;
  struct ZiposUri zipname;
  e = errno;
  if (_weaken(__zipos_open) &&
      _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    res = false;
  } else if (IsMetal()) {
    res = false;
  } else if (!IsWindows()) {
    if (__sys_fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW) != -1) {
      res = S_ISLNK(METASTAT(st, st_mode));
    } else {
      res = false;
    }
  } else {
    res = issymlink_nt(path);
  }
  STRACE("%s(%#s) → %hhhd% m", "issymlink", path, res);
  if (!res && (errno == ENOENT || errno == ENOTDIR)) {
    errno = e;
  }
  return res;
}
