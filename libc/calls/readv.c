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
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Reads data to multiple buffers.
 *
 * @return number of bytes actually read, or -1 w/ errno
 */
ssize_t readv(int fd, const struct iovec *iov, int iovlen) {
  if (fd < 0) return einval();
  if (iovlen < 0) return einval();
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    return weaken(__zipos_read)(
        (struct ZiposHandle *)(intptr_t)g_fds.p[fd].handle, iov, iovlen, -1);
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdSerial) {
    return readv$serial(&g_fds.p[fd], iov, iovlen);
  } else if (!IsWindows()) {
    return readv$sysv(fd, iov, iovlen);
  } else if (fd < g_fds.n &&
             (g_fds.p[fd].kind == kFdFile || g_fds.p[fd].kind == kFdConsole)) {
    return read$nt(&g_fds.p[fd], iov, iovlen, -1);
  } else if (fd < g_fds.n && (g_fds.p[fd].kind == kFdSocket)) {
    return weaken(recvfrom$nt)(&g_fds.p[fd], iov, iovlen, 0, NULL, 0);
  } else {
    return ebadf();
  }
}
