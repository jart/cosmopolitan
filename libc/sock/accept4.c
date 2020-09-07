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
 * Creates client socket file descriptor for incoming connection.
 *
 * @param fd is the server socket file descriptor
 * @param out_addr will receive the remote address
 * @param inout_addrsize provides and receives out_addr's byte length
 * @param flags can have SOCK_{CLOEXEC,NONBLOCK}, which may apply to
 *     both the newly created socket and the server one
 * @return client fd which needs close(), or -1 w/ errno
 */
int accept4(int fd, void *out_addr, uint32_t *inout_addrsize, int flags) {
  if (!out_addr) return efault();
  if (!inout_addrsize) return efault();
  if (!IsWindows()) {
    return accept4$sysv(fd, out_addr, inout_addrsize, flags);
  } else if (isfdkind(fd, kFdSocket)) {
    return accept$nt(&g_fds.p[fd], out_addr, inout_addrsize, flags);
  } else {
    return ebadf();
  }
}
