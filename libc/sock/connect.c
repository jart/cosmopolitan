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
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/dce.h"
#include "libc/intrin/fds.h"
#include "libc/intrin/strace.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Connects socket to remote end.
 *
 * When `fd` is in `O_NONBLOCK` mode, this raises `EINPROGRESS`. To wait
 * for establishment poll() function may be called using `POLLOUT`. Then
 * `SO_ERROR` may be used to check for errors.
 *
 * Connectionless sockets, e.g. UDP, can be connected too. The benefit
 * is not needing to specify the remote address on each send. It also
 * means getsockname() can be called to retrieve routing details.
 *
 * On Linux, your `SO_SNDTIMEO` will timeout connect(). Other OSes (i.e.
 * Windows, MacOS, and BSDs) do not support this and will block forever.
 *
 * On Windows, when this function blocks, there may be a 10 millisecond
 * delay on the handling of signals or thread cancelation.
 *
 * @return 0 on success or -1 w/ errno
 * @raise EINPROGRESS if `O_NONBLOCK` and connecting process initiated
 * @raise EALREADY if a `O_NONBLOCK` connecting already in flight
 * @raise EADDRINUSE if local address is already in use
 * @raise EINTR if a signal handler was called instead
 * @raise ENETUNREACH if network is unreachable
 * @raise ETIMEDOUT if connection timed out
 * @raise EISCONN if already connected
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO)
 */
int connect(int fd, const struct sockaddr *addr, uint32_t addrsize) {
  int rc;
  BEGIN_CANCELATION_POINT;

  if (addr) {
    if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
      rc = enotsock();
    } else if (!IsWindows()) {
      rc = sys_connect(fd, addr, addrsize);
    } else if (!__isfdopen(fd)) {
      rc = ebadf();
    } else if (__isfdkind(fd, kFdSocket)) {
      rc = sys_connect_nt(&g_fds.p[fd], addr, addrsize);
    } else {
      rc = enotsock();
    }
  } else {
    rc = efault();
  }

  END_CANCELATION_POINT;
  STRACE("connect(%d, %s) → %d% lm", fd, DescribeSockaddr(addr, addrsize), rc);
  return rc;
}
