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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/sock/internal.h"

/**
 * Performs recv(), recvfrom(), or readv() on Windows NT.
 *
 * @param fd must be a socket
 * @return number of bytes received, or -1 w/ errno
 */
textwindows ssize_t recvfrom$nt(struct Fd *fd, const struct iovec *iov,
                                size_t iovlen, uint32_t flags,
                                void *opt_out_srcaddr,
                                uint32_t *opt_inout_srcaddrsize) {
  uint32_t got;
  struct iovec$nt iovnt[16];
  got = 0;
  if (WSARecvFrom(fd->handle, iovnt, iovec2nt(iovnt, iov, iovlen), &got, &flags,
                  opt_out_srcaddr, opt_inout_srcaddrsize, NULL, NULL) != -1) {
    return got;
  } else {
    return winsockerr();
  }
}
