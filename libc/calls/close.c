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
#include "libc/macros.h"
#include "libc/sock/internal.h"
#include "libc/sysv/errfuns.h"
#include "libc/zipos/zipos.internal.h"

/**
 * Closes file descriptor.
 *
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int close(int fd) {
  int rc;
  if (fd < 0) return einval();
  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = weaken(__zipos_close)(fd);
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdEpoll) {
    rc = weaken(close$epoll)(fd);
  } else if (!IsWindows()) {
    rc = close$sysv(fd);
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdSocket) {
    rc = weaken(closesocket$nt)(fd);
  } else if (fd < g_fds.n &&
             (g_fds.p[fd].kind == kFdFile || g_fds.p[fd].kind == kFdConsole)) {
    rc = close$nt(fd);
  } else {
    rc = ebadf();
  }
  if (fd < g_fds.n) {
    g_fds.p[fd].kind = kFdEmpty;
    g_fds.f = MIN(g_fds.f, fd);
  }
  return rc;
}
