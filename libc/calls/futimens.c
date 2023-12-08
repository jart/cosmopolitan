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
#include "libc/calls/internal.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timespec.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Changes access/modified time on open file, the modern way.
 *
 * XNU only has microsecond (1e-6) accuracy. Windows only has
 * hectonanosecond (1e-7) accuracy. RHEL5 (Linux c. 2007) doesn't
 * support this system call.
 *
 * @param fd is file descriptor of file whose timestamps will change
 * @param ts is {access, modified} timestamps, or null for current time
 * @return 0 on success, or -1 w/ errno
 * @raise EINVAL if `flags` had an unrecognized value
 * @raise EPERM if pledge() is in play without `fattr` promise
 * @raise EINVAL if `ts` specifies a nanosecond value that's out of range
 * @raise EROFS if `fd` is a zip file or on a read-only filesystem
 * @raise EBADF if `fd` isn't an open file descriptor
 * @raise EFAULT if `ts` memory was invalid
 * @raise ENOSYS on RHEL5 or bare metal
 * @asyncsignalsafe
 */
int futimens(int fd, const struct timespec ts[2]) {
  int rc;
  if (fd < 0) {
    rc = ebadf();  // so we don't confuse __utimens if caller passes AT_FDCWD
  } else {
    rc = __utimens(fd, 0, ts, 0);
  }
  STRACE("futimens(%d, {%s, %s}) → %d% m", fd, DescribeTimespec(0, ts),
         DescribeTimespec(0, ts ? ts + 1 : 0), rc);
  return rc;
}
