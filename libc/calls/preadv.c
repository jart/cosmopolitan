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
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/asan.internal.h"
#include "libc/macros.internal.h"
#include "libc/sysv/consts/iov.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

#define __NR_preadv_linux 0x0127

/**
 * Reads with maximum generality.
 *
 * @return number of bytes actually read, or -1 w/ errno
 * @asyncsignalsafe
 * @vforksafe
 */
ssize_t preadv(int fd, struct iovec *iov, int iovlen, int64_t off) {
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

  /*
   * NT, XNU, and 2007-era Linux don't support this system call.
   */
  if (!once) {
    err = errno;
    rc = sys_preadv(fd, iov, iovlen, off, off);
    if (rc == -1 && errno == ENOSYS) {
      errno = err;
      once = true;
      demodernize = true;
    } else if (IsLinux() && rc == __NR_preadv_linux) {
      if (__iovec_size(iov, iovlen) < __NR_preadv_linux) {
        demodernize = true; /*RHEL5:CVE-2010-3301*/
        once = true;
      } else {
        return rc;
      }
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
