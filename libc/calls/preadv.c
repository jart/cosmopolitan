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
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/iov.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

static ssize_t Preadv(int fd, struct iovec *iov, int iovlen, int64_t off) {
  static bool once, demodernize;
  int i, err;
  ssize_t rc;
  size_t got, toto;

  if (fd < 0) return einval();
  if (iovlen < 0) return einval();
  if (IsAsan() && !__asan_is_valid_iov(iov, iovlen)) return efault();
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    return weaken(__zipos_read)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, iov, iovlen, off);
  } else if (IsWindows()) {
    if (fd < g_fds.n) {
      return sys_read_nt(g_fds.p + fd, iov, iovlen, off);
    } else {
      return ebadf();
    }
  } else if (IsMetal()) {
    return enosys();
  }

  if (iovlen == 1) {
    return sys_pread(fd, iov[0].iov_base, iov[0].iov_len, off, off);
  }

  /*
   * NT, 2018-era XNU, and 2007-era Linux don't support this system call
   */
  if (!__vforked && !once) {
    err = errno;
    rc = sys_preadv(fd, iov, iovlen, off, off);
    if (rc == -1 && errno == ENOSYS) {
      errno = err;
      once = true;
      demodernize = true;
      STRACE("demodernizing %s() due to %s", "preadv", "ENOSYS");
    } else {
      once = true;
      return rc;
    }
  }

  if (!demodernize) {
    return sys_preadv(fd, iov, iovlen, off, off);
  }

  if (!iovlen) {
    return sys_pread(fd, NULL, 0, off, off);
  }

  for (toto = i = 0; i < iovlen; ++i) {
    rc = sys_pread(fd, iov[i].iov_base, iov[i].iov_len, off, off);
    if (rc == -1) {
      if (toto && (errno == EINTR || errno == EAGAIN)) {
        return toto;
      } else {
        return -1;
      }
    }
    got = rc;
    toto += got;
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
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t preadv(int fd, struct iovec *iov, int iovlen, int64_t off) {
  ssize_t rc;
  rc = Preadv(fd, iov, iovlen, off);
#if defined(SYSDEBUG) && _DATATRACE
  if (__strace > 0) {
    if (rc == -1 && errno == EFAULT) {
      STRACE("preadv(%d, %p, %d, %'ld) → %'zd% m", fd, iov, iovlen, off, rc);
    } else {
      kprintf(STRACE_PROLOGUE "preadv(%d, [", fd);
      __strace_iov(iov, iovlen, rc != -1 ? rc : 0);
      kprintf("], %d, %'ld) → %'ld% m%n", iovlen, off, rc);
    }
  }
#endif
  return rc;
}
