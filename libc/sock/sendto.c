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
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Sends data over network.
 *
 * This function blocks unless MSG_DONTWAIT is passed. In that case, the
 * non-error EWOULDBLOCK might be returned. It basically means we didn't
 * wait around to learn an amount of bytes were written that we know in
 * advance are guaranteed to be atomic.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is the data to send, which we'll copy if necessary
 * @param size is the byte-length of buf
 * @param flags MSG_OOB, MSG_DONTROUTE, MSG_PARTIAL, MSG_NOSIGNAL, etc.
 * @param opt_addr is a binary ip:port destination override, which is
 *     mandatory for UDP if connect() wasn't called
 * @param addrsize is the byte-length of addr's true polymorphic form
 * @return number of bytes transmitted, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @asyncsignalsafe
 */
ssize_t sendto(int fd, const void *buf, size_t size, uint32_t flags,
               const void *opt_addr, uint32_t addrsize) {
  static_assert(sizeof(struct sockaddr_in) == sizeof(struct sockaddr_in$bsd));
  if (!IsWindows()) {
    if (!IsBsd() || !opt_addr) {
      return sendto$sysv(fd, buf, size, flags, opt_addr, addrsize);
    } else {
      struct sockaddr_in$bsd addr2;
      if (addrsize != sizeof(addr2)) return einval();
      memcpy(&addr2, opt_addr, sizeof(struct sockaddr_in));
      sockaddr2bsd(&addr2);
      return sendto$sysv(fd, buf, size, flags, &addr2, addrsize);
    }
  } else if (isfdkind(fd, kFdSocket)) {
    return sendto$nt(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1, flags,
                     opt_addr, &addrsize);
  } else {
    return ebadf();
  }
}
