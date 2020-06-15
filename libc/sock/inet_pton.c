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
#include "libc/fmt/fmt.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/consts/inaddr.h"

/**
 * Converts internet address string to binary.
 *
 * @param af can be AF_INET
 * @param src is the ASCII-encoded address
 * @param dst is where the binary-encoded net-order address goes
 * @return 1 on success, 0 on src malformed, or -1 w/ errno
 */
int inet_pton(int af, const char *src, void *dst) {
  if (af == AF_INET) {
    unsigned char *p = (unsigned char *)dst;
    if (sscanf(src, "%hhu.%hhu.%hhu.%hhu", &p[0], &p[1], &p[2], &p[3]) == 4) {
      return 1;
    } else {
      *(uint32_t *)dst = htonl(INADDR_TESTNET3);
      return 0;
    }
  } else {
    *(uint32_t *)dst = htonl(INADDR_TESTNET3);
    return eafnosupport();
  }
}
