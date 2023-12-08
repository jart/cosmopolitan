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
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"

int sys_openat(int dirfd, const char *file, int flags, unsigned mode) {
  static bool once, modernize;
  int d, e, f;
  // RHEL5 doesn't support O_CLOEXEC. It's hard to test for this.
  // Sometimes the system call succeeds and it just doesn't set the
  // flag. Other times, it return -530 which makes no sense.
  if (!IsLinux() || !(flags & O_CLOEXEC) || modernize) {
    d = __sys_openat(dirfd, file, flags, mode);
  } else if (once) {
    if ((d = __sys_openat(dirfd, file, flags & ~O_CLOEXEC, mode)) != -1) {
      e = errno;
      if (__sys_fcntl(d, F_SETFD, FD_CLOEXEC) == -1) {
        errno = e;
      }
    }
  } else {
    e = errno;
    if ((d = __sys_openat(dirfd, file, flags, mode)) != -1) {
      if ((f = __sys_fcntl(d, F_GETFD)) != -1) {
        if (f & FD_CLOEXEC) {
          modernize = true;
        } else {
          __sys_fcntl(d, F_SETFD, FD_CLOEXEC);
        }
      }
      errno = e;
      once = true;
    } else if (errno > 255) {
      once = true;
      d = sys_openat(dirfd, file, flags, mode);
    }
  }
  return d;
}
