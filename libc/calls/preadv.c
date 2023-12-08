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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/likely.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/sysv/errfuns.h"

static ssize_t Preadv(int fd, struct iovec *iov, int iovlen, int64_t off) {
  int e, i;
  size_t got;
  ssize_t rc, toto;

  if (fd < 0) {
    return ebadf();
  }

  if (iovlen < 0) {
    return einval();
  }

  if (IsAsan() && !__asan_is_valid_iov(iov, iovlen)) {
    return efault();
  }

  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    return _weaken(__zipos_read)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, iov, iovlen, off);
  }

  if (IsMetal()) {
    return espipe();  // must be serial or console if not zipos
  }

  if (IsWindows()) {
    if (fd < g_fds.n) {
      if (g_fds.p[fd].kind == kFdSocket) {
        return espipe();
      } else {
        return sys_read_nt(fd, iov, iovlen, off);
      }
    } else {
      return ebadf();
    }
  }

  while (iovlen && !iov->iov_len) {
    --iovlen;
    ++iov;
  }

  if (!iovlen) {
    return sys_pread(fd, 0, 0, off, off);
  }

  if (iovlen == 1) {
    return sys_pread(fd, iov->iov_base, iov->iov_len, off, off);
  }

  e = errno;
  rc = sys_preadv(fd, iov, iovlen, off, off);
  if (rc != -1 || errno != ENOSYS) return rc;
  errno = e;

  for (toto = i = 0; i < iovlen; ++i) {
    rc = sys_pread(fd, iov[i].iov_base, iov[i].iov_len, off, off);
    if (rc == -1) {
      if (!toto) {
        toto = -1;
      } else if (errno != EINTR) {
        notpossible;
      }
      break;
    }
    got = rc;
    toto += got;
    off += got;
    if (got != iov[i].iov_len) {
      break;
    }
  }

  return toto;
}

/**
 * Reads with maximum generality.
 *
 * @return number of bytes actually read, or -1 w/ errno
 * @cancelationpoint
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t preadv(int fd, struct iovec *iov, int iovlen, int64_t off) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;
  rc = Preadv(fd, iov, iovlen, off);
  END_CANCELATION_POINT;
  STRACE("preadv(%d, [%s], %d, %'ld) → %'ld% m", fd,
         DescribeIovec(rc, iov, iovlen), iovlen, off, rc);
  return rc;
}
