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
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

/**
 * Connects socket to remote end.
 *
 * ProTip: Connectionless sockets, e.g. UDP, can be connected too. The
 * benefit is not needing to specify the remote address on each send. It
 * also means getsockname() can be called to retrieve routing details.
 *
 * @return 0 on success or -1 w/ errno
 * @asyncsignalsafe
 */
int connect(int fd, const void *addr, uint32_t addrsize) {
  if (!IsWindows()) {
    return connect$sysv(fd, addr, addrsize);
  } else if (isfdkind(fd, kFdSocket)) {
    return connect$nt(&g_fds.p[fd], addr, addrsize);
  } else {
    return ebadf();
  }
}
