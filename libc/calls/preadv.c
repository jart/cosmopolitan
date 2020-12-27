/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/bits/bits.h"
#include "libc/bits/safemacros.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/sysv/consts/iov.h"
#include "libc/sysv/errfuns.h"

#define __NR_preadv_linux 0x0127

/**
 * Reads data from multiple buffers from file descriptor at offset.
 *
 * @param count is recommended to be 16 or fewer; if it exceeds IOV_MAX
 *     then the extra buffers are simply ignored
 * @return number of bytes actually read, or -1 w/ errno
 */
ssize_t preadv(int fd, struct iovec *iovec, int count, int64_t off) {
  static bool once, demodernize;
  int olderr;
  ssize_t rc;
  if (!count) return 0;
  if ((count = min(count, IOV_MAX)) < 0) return einval();

  /*
   * NT, XNU, and 2007-era Linux don't support this system call.
   */
  if (!once) {
    once = true;
    if (IsModeDbg() || (IsLinux() && iovec->iov_len >= __NR_preadv_linux)) {
      /*
       * Read size is too large to detect older kernels safely without
       * introducing nontrivial mechanics. We'll try again later.
       */
      once = false;
      demodernize = true;
    } else {
      olderr = errno;
      rc = preadv$sysv(fd, iovec, count, off);
      if (rc == -1 && errno == ENOSYS) {
        errno = olderr;
        demodernize = true;
      } else if (IsLinux() && rc == __NR_preadv_linux /*RHEL5:CVE-2010-3301*/) {
        demodernize = true;
      } else {
        return rc;
      }
    }
  }

  if (!demodernize) {
    return preadv$sysv(fd, iovec, count, off);
  } else {
    return pread(fd, iovec[0].iov_base, iovec[0].iov_len, off);
  }
}
