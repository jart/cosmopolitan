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
#include "libc/fmt/itoa.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/mem/alloca.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

static const char *DescribeUnlinkatFlags(char buf[12], int flags) {
  if (!flags)
    return "0";
  if (flags & AT_REMOVEDIR)
    return "AT_REMOVEDIR";
  FormatInt32(buf, flags);
  return buf;
}

/**
 * Deletes inode and maybe the file too.
 *
 * This may be used to delete files and directories and symlinks.
 *
 * You can call unlink before calling close, in which case the file will
 * continue to exist except it'll have no name. This is even the case on
 * Windows because Cosmopolitan always opens files w/ delete permission.
 * However Windows has a quirk where files can't be deleted before close
 * if they have memory mappings.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     path is relative, then path becomes relative to dirfd
 * @param path is the thing to delete
 * @param flags can have AT_REMOVEDIR
 * @return 0 on success, or -1 w/ errno
 * @raise ENOENT if `path` is empty
 * @raise ENOENT if `path` doesn't exist
 * @raise EROFS if either path is under /zip/...
 * @raise EFAULT if `path` points to invalid memory
 * @raise EISDIR if `path` is a dir when `flags` is zero
 * @raise EINVAL if `flags` has unsupported or unknown flags
 * @raise EACCES on Windows if someone in chdir()'d into directory
 * @raise ENOTDIR if `path` isn't a dir when `flags` has `AT_REMOVEDIR`
 * @raise ENOTDIR if a directory component in `path` exists as non-directory
 * @raise ENOTDIR if `path` ends with a trailing slash and refers to a file
 * @raise ENOTDIR if `path` is relative and `dirfd` isn't an open directory
 * @raise ENOTDIR if `path` isn't a directory and `O_DIRECTORY` was passed
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EILSEQ if `path` contains illegal UTF-8 sequences (Windows/MacOS)
 */
int unlinkat(int dirfd, const char *path, int flags) {
  int rc;

  if (kisdangerous(path)) {
    rc = efault();
  } else if (_weaken(__zipos_notat) &&
             (rc = __zipos_notat(dirfd, path)) == -1) {
    rc = erofs();
  } else if (!IsWindows()) {
    rc = sys_unlinkat(dirfd, path, flags);
  } else {
    rc = sys_unlinkat_nt(dirfd, path, flags);
  }

  // POSIX.1 says unlink(directory) raises EPERM but on Linux
  // it always raises EISDIR, which is so much less ambiguous
  int e = errno;
  if (!IsLinux() && !IsWindows() && rc == -1 && !flags &&
      (e == EPERM || e == EACCES)) {
    struct stat st;
    if (!fstatat(dirfd, path, &st, 0) && S_ISDIR(st.st_mode)) {
      errno = EISDIR;
    } else {
      errno = e;
    }
  }

  STRACE("unlinkat(%s, %#s, %s) → %d% m", DescribeDirfd(dirfd), path,
         DescribeUnlinkatFlags(alloca(12), flags), rc);
  return rc;
}
