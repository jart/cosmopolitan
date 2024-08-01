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
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

/**
 * Deletes inode and maybe the file too.
 *
 * This may be used to delete files and directories and symlinks.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     path is relative, then path becomes relative to dirfd
 * @param path is the thing to delete
 * @param flags can have AT_REMOVEDIR
 * @return 0 on success, or -1 w/ errno
 */
int unlinkat(int dirfd, const char *path, int flags) {
  int rc;

  if (_weaken(__zipos_notat) && (rc = __zipos_notat(dirfd, path)) == -1) {
    STRACE("zipos unlinkat not supported yet");
  } else if (!IsWindows()) {
    rc = sys_unlinkat(dirfd, path, flags);
  } else {
    rc = sys_unlinkat_nt(dirfd, path, flags);
  }

  // POSIX.1 says unlink(directory) raises EPERM but on Linux
  // it always raises EISDIR, which is so much less ambiguous
  int e = errno;
  if (!IsLinux() && rc == -1 && !flags && (e == EPERM || e == EACCES)) {
    struct stat st;
    if (!fstatat(dirfd, path, &st, 0) && S_ISDIR(st.st_mode)) {
      errno = EISDIR;
    } else {
      errno = e;
    }
  }

  STRACE("unlinkat(%s, %#s, %#b) → %d% m", DescribeDirfd(dirfd), path, flags,
         rc);
  return rc;
}
