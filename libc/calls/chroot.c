/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes root directory.
 *
 * Please consider using unveil() instead of chroot(). If you use this
 * system call then consider using both chdir() and closefrom() before
 * calling this function. Otherwise there's a small risk that fchdir()
 * could be used to escape the chroot() environment. Cosmopolitan Libc
 * focuses on static binaries which make chroot() infinitely easier to
 * use since you don't need to construct an entire userspace each time
 * however unveil() is still better to use on modern Linux and OpenBSD
 * because it doesn't require root privileges.
 *
 * @param path shall become the new root directory
 * @return 0 on success, or -1 w/ errno
 * @raise EACCES if we don't have permission to search a component of `path`
 * @raise ENOTDIR if a directory component in `path` exists as non-directory
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EPERM if not root or pledge() is in play
 * @raise EIO if a low-level i/o error occurred
 * @raise EFAULT if `path` is bad memory
 * @raise ENOENT if `path` doesn't exist
 * @raise ENOSYS on Windows
 */
int chroot(const char *path) {
  int rc;
  if (!path || (IsAsan() && !__asan_is_valid_str(path))) {
    rc = efault();
  } else {
    rc = sys_chroot(path);
  }
  STRACE("chroot(%s) → %d% m", path, rc);
  return rc;
}
