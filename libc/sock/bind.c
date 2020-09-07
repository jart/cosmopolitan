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
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Assigns local address and port number to socket.
 *
 * @param fd is the file descriptor returned by socket()
 * @param addr is usually the binary-encoded ip:port on which to listen
 * @param addrsize is the byte-length of addr's true polymorphic form
 * @return socket file descriptor or -1 w/ errno
 * @error ENETDOWN, EPFNOSUPPORT, etc.
 * @asyncsignalsafe
 */
int bind(int fd, const void *addr, uint32_t addrsize) {
  if (!addr) return efault();
  if (addrsize == sizeof(struct sockaddr_in)) {
    if (!IsWindows()) {
      if (!IsBsd()) {
        return bind$sysv(fd, addr, addrsize);
      } else {
        struct sockaddr_in$bsd addr2;
        static_assert(sizeof(struct sockaddr_in) ==
                      sizeof(struct sockaddr_in$bsd));
        memcpy(&addr2, addr, sizeof(struct sockaddr_in));
        sockaddr2bsd(&addr2);
        return bind$sysv(fd, &addr2, addrsize);
      }
    } else if (isfdkind(fd, kFdSocket)) {
      return bind$nt(&g_fds.p[fd], addr, addrsize);
    } else {
      return ebadf();
    }
  } else {
    return einval();
  }
}
