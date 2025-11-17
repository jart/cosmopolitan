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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

int sys_fchmodat_linux(int, const char *, unsigned, int);
int sys_fchmodat2(int, const char *, unsigned, int);

/**
 * Changes permissions on file, e.g.:
 *
 *     CHECK_NE(-1, fchmodat(AT_FDCWD, "foo/bar.txt", 0644));
 *     CHECK_NE(-1, fchmodat(AT_FDCWD, "o/default/program", 0755));
 *     CHECK_NE(-1, fchmodat(AT_FDCWD, "privatefolder/", 0700));
 *
 * @param path must exist
 * @param mode contains octal flags (base 8)
 * @param flags can have `AT_SYMLINK_NOFOLLOW`
 * @raise EROFS if `dirfd` or `path` use zip file system
 * @raise EOPNOTSUP on Linux if `path` is a symbolic link, `AT_SYMLINK_NOFOLLOW`
 *     is set in `flags`, and filesystem does not support setting the mode of
 *     symbolic links.
 * @raise EACCES if denied access to component in path prefix
 * @raise EIO if i/o error occurred while reading from filesystem
 * @raise ELOOP if a symbolic link loop exists in `path`
 * @raise ENAMETOOLONG if a component in `path` exceeds `NAME_MAX`
 * @raise EPERM if geteuid() doesn't match owner of file and not root
 * @raise ENOENT on empty string or if component in path doesn't exist
 * @raise ENOTDIR if a parent component existed that wasn't a directory
 * @raise ENOTDIR if `path` is relative and `dirfd` isn't an open directory
 * @raise ENOMEM if insufficient memory was available to the implementation
 * @raise EILSEQ on Windows if `path` has bad utf-8 of control characters
 * @raise EROFS if `path` points to the `/zip/...` synthetic filesystem
 * @raise ELOOP may ahappen if `SYMLOOP_MAX` symlinks were dereferenced
 * @raise ENAMETOOLONG if resolved `path` exceeded `PATH_MAX`
 * @raise EROFS if `path` resides on a real-only filesystem
 * @raise EFAULT if `path` points to invalid memory
 * @raise EINVAL if `flags` has unsupported bits
 * @asyncsignalsafe
 * @see fchmod()
 */
int fchmodat(int dirfd, const char *path, uint32_t mode, int flags) {
  int rc;
  if (kisdangerous(path)) {
    rc = efault();
  } else if (_weaken(__zipos_notat) &&
             (rc = __zipos_notat(dirfd, path)) == -1) {
    rc = erofs();
  } else if (!IsWindows()) {
    if (IsLinux() && flags) {
      int serrno = errno;
      rc = sys_fchmodat2(dirfd, path, mode, flags);
      if (rc == -1 && errno == ENOSYS) {
        errno = serrno;
        BLOCK_CANCELATION;
        rc = sys_fchmodat_linux(dirfd, path, mode, flags);
        ALLOW_CANCELATION;
      }
    } else {
      rc = sys_fchmodat(dirfd, path, mode, flags);
    }
  } else {
    rc = sys_fchmodat_nt(dirfd, path, mode, flags);
  }
  STRACE("fchmodat(%s, %#s, %#o, %d) → %d% m", DescribeDirfd(dirfd), path, mode,
         flags, rc);
  return rc;
}
