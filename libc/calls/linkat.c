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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/errfuns.h"
#include "libc/runtime/zipos.internal.h"

/**
 * Creates hard filesystem link.
 *
 * This allows two names to point to the same file data on disk. They
 * can only be differentiated by examining the inode number.
 *
 * @param flags can have AT_EMPTY_PATH or AT_SYMLINK_NOFOLLOW
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath,
           int flags) {
  int rc;
  if (IsAsan() &&
      (!__asan_is_valid_str(oldpath) || !__asan_is_valid_str(newpath))) {
    rc = efault();
  } else if (_weaken(__zipos_notat) &&
             ((rc = __zipos_notat(olddirfd, oldpath)) == -1 ||
              (rc = __zipos_notat(newdirfd, newpath)) == -1)) {
    STRACE("zipos fchownat not supported yet");
  } else if (!IsWindows()) {
    rc = sys_linkat(olddirfd, oldpath, newdirfd, newpath, flags);
  } else {
    rc = sys_linkat_nt(olddirfd, oldpath, newdirfd, newpath);
  }
  STRACE("linkat(%s, %#s, %s, %#s, %#b) → %d% m", DescribeDirfd(olddirfd),
         oldpath, DescribeDirfd(newdirfd), newpath, flags, rc);
  return rc;
}
