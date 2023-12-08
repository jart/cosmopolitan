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
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/consts/af.h"

/**
 * Creates bidirectional pipe, e.g.
 *
 *     int sv[2];
 *     socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
 *
 * @param family should be AF_UNIX or synonymously AF_LOCAL
 * @param type can be SOCK_STREAM or SOCK_DGRAM and additionally,
 *     may be or'd with SOCK_NONBLOCK, SOCK_CLOEXEC
 * @param sv a vector of 2 integers to store the created sockets
 * @return 0 if success, -1 in case of error
 * @error EFAULT, EPFNOSUPPORT, etc.
 * @see libc/sysv/consts.sh
 * @asyncsignalsafe
 */
int socketpair(int family, int type, int protocol, int sv[2]) {
  if (family == AF_UNSPEC) family = AF_UNIX;
  if (!IsWindows()) {
    return sys_socketpair(family, type, protocol, sv);
  } else {
    return sys_socketpair_nt(family, type, protocol, sv);
  }
}
