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

/**
 * Formats internet address to string.
 *
 * @param af can be AF_INET
 * @param src is the binary-encoded address, e.g. &addr->sin_addr
 * @param dst is the output string buffer
 * @param size is bytes in dst, which needs 16+ for IPv4
 * @return dst on success or NULL w/ errno
 */
const char *inet_ntop(int af, const void *src, char *dst, uint32_t size) {
  if (src) {
    if (af == AF_INET) {
      unsigned char *p = (unsigned char *)src;
      if (snprintf(dst, size, "%hhu.%hhu.%hhu.%hhu", p[0], p[1], p[2], p[3]) <
          size) {
        return dst;
      } else {
        enospc();
      }
    } else {
      eafnosupport();
    }
  } else {
    einval();
  }
  if (size) dst[0] = '\0';
  return NULL;
}
