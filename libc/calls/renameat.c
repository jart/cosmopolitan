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
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/errfuns.h"
#include "libc/runtime/zipos.internal.h"

/**
 * Renames files relative to directories.
 *
 * This is generally an atomic operation with the file system, since all
 * it's doing is changing a name associated with an inode. However, that
 * means rename() doesn't permit your `oldpathname` and `newpathname` to
 * be on separate file systems, in which case this returns EXDEV. That's
 * also the case on Windows.
 *
 * @param olddirfd is normally AT_FDCWD but if it's an open directory
 *     and oldpath is relative, then oldpath become relative to dirfd
 * @param newdirfd is normally AT_FDCWD but if it's an open directory
 *     and newpath is relative, then newpath become relative to dirfd
 * @return 0 on success, or -1 w/ errno
 */
int renameat(int olddirfd, const char *oldpath, int newdirfd,
             const char *newpath) {
  int rc;
  if (IsAsan() &&
      (!__asan_is_valid_str(oldpath) || !__asan_is_valid_str(newpath))) {
    rc = efault();
  } else if (_weaken(__zipos_notat) &&
             ((rc = __zipos_notat(olddirfd, oldpath)) == -1 ||
              (rc = __zipos_notat(newdirfd, newpath)) == -1)) {
    STRACE("zipos renameat not supported yet");
  } else if (!IsWindows()) {
    rc = sys_renameat(olddirfd, oldpath, newdirfd, newpath);
  } else {
    rc = sys_renameat_nt(olddirfd, oldpath, newdirfd, newpath);
  }
  STRACE("renameat(%s, %#s, %s, %#s) → %d% m", DescribeDirfd(olddirfd), oldpath,
         DescribeDirfd(newdirfd), newpath, rc);
  return rc;
}
