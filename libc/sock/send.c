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
#include "libc/sock/sock.h"

/**
 * Sends data to network socket.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is the data to send, which we'll copy if necessary
 * @param size is the byte-length of buf
 * @param flags MSG_OOB, MSG_DONTROUTE, MSG_PARTIAL, MSG_NOSIGNAL, etc.
 * @return number of bytes transmitted, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @asyncsignalsafe
 */
ssize_t send(int fd, const void *buf, size_t size, int flags) {
  return sendto(fd, buf, size, flags, NULL, 0);
}
