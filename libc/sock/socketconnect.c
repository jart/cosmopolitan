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
#include "libc/calls/calls.h"
#include "libc/dns/dns.h"
#include "libc/errno.h"
#include "libc/sock/sock.h"

/**
 * Opens socket client connection.
 *
 * @param ai can be obtained via getaddrinfo()
 * @return socket file descriptor, or -1 w/ errno
 */
int socketconnect(const struct addrinfo *ai, int flags) {
  int fd;
  fd = socket(ai->ai_family, ai->ai_socktype | flags, ai->ai_protocol);
  if (fd != -1) {
    if (connect(fd, ai->ai_addr, ai->ai_addrlen) != -1 ||
        errno == EINPROGRESS) {
      return fd;
    }
    close(fd);
  }
  return -1;
}
