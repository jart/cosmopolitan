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
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Sets access/modified time on file, the modern way.
 *
 * XNU only has microsecond (1e-6) accuracy and there's no
 * `dirfd`-relative support. Windows only has hectonanosecond (1e-7)
 * accuracy. RHEL5 doesn't support `dirfd` or `flags` and will truncate
 * timestamps to seconds.
 *
 * If you'd rather specify an open file descriptor rather than its
 * filesystem path, then consider using futimens().
 *
 * @param dirfd can be `AT_FDCWD` or an open directory
 * @param path is filename whose timestamps should be modified
 * @param ts is {access, modified} timestamps, or null for current time
 * @param flags can have `AT_SYMLINK_NOFOLLOW`
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `flags` had an unrecognized value
 * @raise EINVAL on XNU or RHEL5 when any `flags` are used
 * @raise EPERM if pledge() is in play without `fattr` promise
 * @raise EACCES if unveil() is in play and `path` isn't unveiled
 * @raise EINVAL if `ts` specifies a nanosecond value that's out of range
 * @raise ENAMETOOLONG if symlink-resolved `path` length exceeds `PATH_MAX`
 * @raise ENAMETOOLONG if component in `path` exists longer than `NAME_MAX`
 * @raise EBADF if `dirfd` isn't a valid fd or `AT_FDCWD`
 * @raise EFAULT if `path` or `ts` memory was invalid
 * @raise EROFS if `path` is on read-only filesystem (e.g. zipos)
 * @raise ENOTSUP on XNU or RHEL5 when `dirfd` isn't `AT_FDCWD`
 * @raise ENOSYS on bare metal
 * @asyncsignalsafe
 */
int utimensat(int dirfd, const char *path, const struct timespec ts[2],
              int flags) {
  int rc;
  if (!path) {
    rc = efault();  // linux kernel abi behavior isn't supported
  } else {
    rc = __utimens(dirfd, path, ts, flags);
  }
  STRACE("utimensat(%s, %#s, {%s, %s}, %#o) → %d% m", DescribeDirfd(dirfd),
         path, DescribeTimespec(0, ts), DescribeTimespec(0, ts ? ts + 1 : 0),
         flags, rc);
  return rc;
}
