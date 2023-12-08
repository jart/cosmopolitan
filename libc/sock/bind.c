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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Assigns local address and port number to socket, e.g.
 *
 *     struct sockaddr_in in = {AF_INET, htons(12345), {htonl(0x7f000001)}};
 *     int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 *     bind(fd, (struct sockaddr *)&in, sizeof(in));
 *
 * On Windows, Cosmopolitan's implementation of bind() takes care of
 * always setting the WIN32-specific `SO_EXCLUSIVEADDRUSE` option on
 * inet stream sockets in order to safeguard your servers from tests
 *
 * @param fd is the file descriptor returned by socket()
 * @param addr is usually the binary-encoded ip:port on which to listen
 * @param addrsize is the byte-length of addr's true polymorphic form
 * @return 0 on success or -1 w/ errno
 * @error ENETDOWN, EPFNOSUPPORT, etc.
 * @asyncsignalsafe
 */
int bind(int fd, const struct sockaddr *addr, uint32_t addrsize) {
  int rc;
  if (!addr || (IsAsan() && !__asan_is_valid(addr, addrsize))) {
    rc = efault();
  } else if (addrsize >= sizeof(struct sockaddr_in)) {
    if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
      rc = enotsock();
    } else if (!IsWindows()) {
      rc = sys_bind(fd, addr, addrsize);
    } else if (!__isfdopen(fd)) {
      rc = ebadf();
    } else if (__isfdkind(fd, kFdSocket)) {
      rc = sys_bind_nt(&g_fds.p[fd], addr, addrsize);
    } else {
      rc = enotsock();
    }
  } else {
    rc = einval();
  }
  STRACE("bind(%d, %s) -> %d% lm", fd, DescribeSockaddr(addr, addrsize), rc);
  return rc;
}
