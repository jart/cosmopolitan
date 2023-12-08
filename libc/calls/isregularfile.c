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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

/**
 * Returns true if file exists and is a regular file.
 *
 * This function is equivalent to:
 *
 *     return fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW) != -1 &&
 *            S_ISREG(st.st_mode);
 *
 * Except faster, with fewer dependencies, and less errno clobbering.
 *
 * @see isdirectory(), ischardev(), issymlink()
 */
bool32 isregularfile(const char *path) {
  int e;
  bool res;
  union metastat st;
  struct ZiposUri zipname;
  e = errno;
  if (IsAsan() && !__asan_is_valid_str(path)) {
    efault();
    res = false;
  } else if (_weaken(__zipos_open) &&
             _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    if (_weaken(__zipos_stat)(&zipname, &st.cosmo) != -1) {
      res = !!S_ISREG(st.cosmo.st_mode);
    } else {
      res = false;
    }
  } else if (IsMetal()) {
    res = false;
  } else if (!IsWindows()) {
    if (__sys_fstatat(AT_FDCWD, path, &st, AT_SYMLINK_NOFOLLOW) != -1) {
      res = S_ISREG(METASTAT(st, st_mode));
    } else {
      res = false;
    }
  } else {
    res = isregularfile_nt(path);
  }
  STRACE("%s(%#s) → %hhhd% m", "isregularfile", path, res);
  if (!res && (errno == ENOENT || errno == ENOTDIR)) {
    errno = e;
  }
  return res;
}
