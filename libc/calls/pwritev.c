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
#include "libc/sysv/errfuns.h"

static ssize_t Pwritev(int fd, const struct iovec *iov, int iovlen,
                       int64_t off) {
  int i, e;
  size_t sent;
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
    return ebadf();
  }

  if (IsWindows()) {
    if (fd < g_fds.n) {
      if (g_fds.p[fd].kind == kFdSocket) {
        return espipe();
      } else {
        return sys_write_nt(fd, iov, iovlen, off);
      }
    } else {
      return ebadf();
    }
  }

  if (IsMetal()) {
    return espipe();  // must be serial or console if not zipos
  }

  while (iovlen && !iov->iov_len) {
    --iovlen;
    ++iov;
  }

  if (!iovlen) {
    return sys_pwrite(fd, 0, 0, off, off);
  }

  if (iovlen == 1) {
    return sys_pwrite(fd, iov->iov_base, iov->iov_len, off, off);
  }

  e = errno;
  rc = sys_pwritev(fd, iov, iovlen, off, off);
  if (rc != -1 || errno != ENOSYS) return rc;
  errno = e;

  for (toto = i = 0; i < iovlen; ++i) {
    rc = sys_pwrite(fd, iov[i].iov_base, iov[i].iov_len, off, off);
    if (rc == -1) {
      if (!toto) {
        toto = -1;
      } else if (errno != EINTR) {
        notpossible;
      }
      break;
    }
    sent = rc;
    toto += sent;
    off += sent;
    if (sent != iov[i].iov_len) {
      break;
    }
  }

  return toto;
}

/**
 * Writes data from multiple buffers to offset.
 *
 * Please note that it's not an error for a short write to happen. This
 * can happen in the kernel if EINTR happens after some of the write has
 * been committed. It can also happen if we need to polyfill this system
 * call using pwrite().
 *
 * @return number of bytes actually sent, or -1 w/ errno
 * @cancelationpoint
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t pwritev(int fd, const struct iovec *iov, int iovlen, int64_t off) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;
  rc = Pwritev(fd, iov, iovlen, off);
  END_CANCELATION_POINT;
  STRACE("pwritev(%d, %s, %d, %'ld) → %'ld% m", fd,
         DescribeIovec(rc != -1 ? rc : -2, iov, iovlen), iovlen, off, rc);
  return rc;
}
