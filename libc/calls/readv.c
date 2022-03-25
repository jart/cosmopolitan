/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/intrin/asan.internal.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Reads data to multiple buffers.
 *
 * @return number of bytes actually read, or -1 w/ errno
 * @restartable
 */
ssize_t readv(int fd, const struct iovec *iov, int iovlen) {
  ssize_t rc;
  if (fd >= 0 && iovlen >= 0) {
    if (IsAsan() && !__asan_is_valid_iov(iov, iovlen)) {
      rc = efault();
    } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
      rc = weaken(__zipos_read)(
          (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, iov, iovlen, -1);
    } else if (!IsWindows() && !IsMetal()) {
      rc = sys_readv(fd, iov, iovlen);
    } else if (fd >= g_fds.n) {
      rc = ebadf();
    } else if (IsMetal()) {
      rc = sys_readv_metal(g_fds.p + fd, iov, iovlen);
    } else {
      rc = sys_readv_nt(g_fds.p + fd, iov, iovlen);
    }
  } else {
    rc = einval();
  }
  STRACE("readv(%d, %p, %d) → %'zd% m", fd, iov, iovlen, rc);
  return rc;
}
