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
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

/**
 * Receives data from network.
 *
 * This function blocks unless MSG_DONTWAIT is passed.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is where received network data gets copied
 * @param size is the byte capacity of buf
 * @param flags can have MSG_{WAITALL,DONTROUTE,PEEK,OOB}, etc.
 * @param opt_out_srcaddr receives the binary ip:port of the data's origin
 * @param opt_inout_srcaddrsize is srcaddr capacity which gets updated
 * @return number of bytes received, 0 on remote close, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @asyncsignalsafe
 */
ssize_t recvfrom(int fd, void *buf, size_t size, uint32_t flags,
                 void *opt_out_srcaddr, uint32_t *opt_inout_srcaddrsize) {
  ssize_t got;
  if (!IsWindows()) {
    got = recvfrom$sysv(fd, buf, size, flags, opt_out_srcaddr,
                        opt_inout_srcaddrsize);
    if (opt_out_srcaddr && IsBsd() && got != -1) {
      sockaddr2linux(opt_out_srcaddr);
    }
    return got;
  } else if (__isfdkind(fd, kFdSocket)) {
    return recvfrom$nt(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1, flags,
                       opt_out_srcaddr, opt_inout_srcaddrsize);
  } else {
    return ebadf();
  }
}
