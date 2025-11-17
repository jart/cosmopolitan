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
#include "libc/calls/internal.h"
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
#include "libc/sysv/consts/ok.h"
#include "libc/sysv/errfuns.h"

#define DescribeAccessMode(amode) _DescribeAccessMode(alloca(13), amode)
static char *_DescribeAccessMode(char buf[13], int amode) {
  switch (amode) {
    case F_OK:
      return "F_OK";
    case R_OK:
      return "R_OK";
    case W_OK:
      return "W_OK";
    case X_OK:
      return "X_OK";
    case R_OK | W_OK:
      return "R_OK|W_OK";
    case R_OK | X_OK:
      return "R_OK|X_OK";
    case W_OK | X_OK:
      return "W_OK|X_OK";
    case R_OK | W_OK | X_OK:
      return "R_OK|W_OK|X_OK";
    default:
      FormatOctal32(buf, amode, true);
      return buf;
  }
}

/**
 * Checks if effective user can access path in particular ways.
 *
 * @param dirfd is normally AT_FDCWD but if it's an open directory and
 *     file is a relative path, then file is opened relative to dirfd
 * @param path is a filename or directory
 * @param amode can be `F_OK` or a mixture of `R_OK`, `W_OK`, and `X_OK`
 * @param flags can have `AT_EACCESS` and/or `AT_SYMLINK_NOFOLLOW`
 * @return 0 if ok, or -1 and sets errno
 * @raise EINVAL if `amode` or `flags` had invalid values
 * @raise EINVAL if `AT_SYMLINK_NOFOLLOW` is used on FreeBSD
 * @raise EPERM if pledge() is in play without rpath promise
 * @raise EACCES if access for requested `amode` would be denied
 * @raise ENOTDIR if a directory component in `path` exists as non-directory
 * @raise EILSEQ on Windows if `path` has bad utf-8 of control characters
 * @raise ENOENT if component of `path` doesn't exist or `path` is empty
 * @raise ENOTSUP if `path` is a zip file and `dirfd` isn't `AT_FDCWD`
 * @note on Linux `flags` is only supported on Linux 5.8+
 * @asyncsignalsafe
 */
int faccessat(int dirfd, const char *path, int amode, int flags) {
  int e, rc;
  struct ZiposUri zipname;
  if (kisdangerous(path)) {
    rc = efault();
  } else if ((flags & ~(AT_SYMLINK_NOFOLLOW | AT_EACCESS)) ||
             !(amode == F_OK || !(amode & ~(R_OK | W_OK | X_OK)))) {
    rc = einval();
  } else if (__isfdkind(dirfd, kFdZip)) {
    rc = enotsup();
  } else if (_weaken(__zipos_open) &&
             _weaken(__zipos_parseuri)(path, &zipname) != -1) {
    rc = _weaken(__zipos_access)(&zipname, amode);
  } else if (!IsWindows()) {
    e = errno;
    if (!flags)
      goto NoFlags;
    if ((rc = sys_faccessat2(dirfd, path, amode, flags)) == -1) {
      if (errno == ENOSYS) {
        errno = e;
      NoFlags:
        rc = sys_faccessat(dirfd, path, amode, flags);
      }
    }
  } else {
    rc = sys_faccessat_nt(dirfd, path, amode, flags);
  }
  STRACE("faccessat(%s, %#s, %s, %#x) → %d% m", DescribeDirfd(dirfd), path,
         DescribeAccessMode(amode), flags, rc);
  return rc;
}
