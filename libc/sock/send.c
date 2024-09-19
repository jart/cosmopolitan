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
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/macros.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sysv/errfuns.h"

/**
 * Sends data to network socket.
 *
 * Calling `send(fd, p, n, 0)` is equivalent to `write(fd, p, n)`.
 *
 * On Windows, calling send() or write() on a socket in `O_NONBLOCK`
 * mode will block. This is done for many reasons. First, most UNIX OSes
 * have a similar behavior, due to how little code checks the return
 * status of write(). Secondly, WIN32 has bugs that prevent us from
 * canceling an overlapped WSASend() operation safely. Programs that
 * want to avoid send() blocking should call poll() beforehand with the
 * POLLOUT flag to test when the socket can safely be written without
 * blocking. It's also possible to pass `MSG_DONTWAIT` via `flags` in
 * which case send() will do this for you automatically.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is the data to send, which we'll copy if necessary
 * @param size is the byte-length of buf
 * @param flags can have `MSG_OOB`, `MSG_DONTROUTE`, and `MSG_DONTWAIT`
 * @return number of bytes transmitted, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_SNDTIMEO on Linux or Windows)
 */
ssize_t send(int fd, const void *buf, size_t size, int flags) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;

  if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotsock();
  } else if (!IsWindows()) {
    rc = sys_sendto(fd, buf, size, flags, 0, 0);
  } else if (__isfdopen(fd)) {
    if (__isfdkind(fd, kFdSocket)) {
      rc = sys_send_nt(fd, (struct iovec[]){{(void *)buf, size}}, 1, flags);
    } else if (__isfdkind(fd, kFdFile)) {
      if (flags) {
        rc = einval();
      } else {
        rc = sys_write_nt(fd, (struct iovec[]){{(void *)buf, size}}, 1, -1);
      }
    } else {
      rc = enotsock();
    }
  } else {
    rc = ebadf();
  }

  END_CANCELATION_POINT;
  DATATRACE("send(%d, %#.*hhs%s, %'zu, %s) → %'ld% lm", fd, MAX(0, MIN(40, rc)),
            buf, rc > 40 ? "..." : "", size, DescribeMsg(flags), rc);
  return rc;
}
