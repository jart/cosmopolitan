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
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates new system resource for network communication.
 *
 * @param family can be AF_UNIX, AF_INET, etc.
 * @param type can be SOCK_STREAM (for TCP), SOCK_DGRAM (e.g. UDP), or
 *     SOCK_RAW (IP) so long as IP_HDRINCL was passed to setsockopt();
 *     and additionally, may be or'd with SOCK_NONBLOCK, SOCK_CLOEXEC
 * @param protocol can be IPPROTO_TCP, IPPROTO_UDP, or IPPROTO_ICMP
 * @return socket file descriptor or -1 w/ errno
 * @error ENETDOWN, EPFNOSUPPORT, etc.
 * @see libc/sysv/consts.sh
 * @asyncsignalsafe
 */
int socket(int family, int type, int protocol) {
  if (family == AF_UNSPEC) {
    family = AF_INET;
  } else if (family == AF_INET6) {
    /* Recommend IPv6 on frontend serving infrastructure only. That's
       what Google Cloud does. It's more secure. It also means poll()
       will work on Windows, which doesn't allow mixing third layers. */
    return epfnosupport();
  }
  if (!IsWindows()) {
    return socket$sysv(family, type, protocol);
  } else {
    return socket$nt(family, type, protocol);
  }
}
