/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"
#include "libc/sysv/errfuns.h"

/**
 * Creates new system resource for network communication, e.g.
 *
 *     int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 *
 * @param family can be AF_UNIX, AF_INET, AF_INET6, etc.
 * @param type can be SOCK_STREAM (for TCP), SOCK_DGRAM (e.g. UDP), or
 *     SOCK_RAW (IP) so long as IP_HDRINCL was passed to setsockopt();
 *     and additionally, may be or'd with SOCK_NONBLOCK, SOCK_CLOEXEC
 * @param protocol can be IPPROTO_TCP, IPPROTO_UDP, or IPPROTO_ICMP
 * @return socket file descriptor or -1 w/ errno
 * @raise EAFNOSUPPORT if `family` isn't supported by system or platform
 * @see libc/sysv/consts.sh
 * @asyncsignalsafe
 */
int socket(int family, int type, int protocol) {
  int rc;
  if (family == AF_UNSPEC) {
    family = AF_INET;
  }
  if (family == -1) {
    rc = eafnosupport();
  } else if (!IsWindows()) {
    rc = sys_socket(family, type, protocol);
  } else {
    rc = sys_socket_nt(family, type, protocol);
  }
  STRACE("socket(%s, %s, %s) → %d% lm", DescribeSocketFamily(family),
         DescribeSocketType(type), DescribeSocketProtocol(protocol), rc);
  return rc;
}
