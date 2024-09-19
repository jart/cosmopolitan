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
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.h"
#include "libc/intrin/strace.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/msg.h"
#include "libc/sysv/errfuns.h"

/**
 * Receives data from network socket.
 *
 * Calling `recv(fd, p, n, 0)` is equivalent to `read(fd, p, n)`.
 *
 * Unlike files where the OS tries very hard to fulfill the entire
 * requested `size` before returning, read operations on sockets aim to
 * return as quickly as possible. For example, if 10 bytes are requested
 * and a packet comes in with only 5 bytes, then recv() will most likely
 * return those 5 bytes before waiting longer. The `MSG_WAITALL` flag
 * may be passed when waiting longer is desired. In that case, short
 * reads should only be possible when the connection status changes or
 * the receive operation is interrupted by a signal.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is where received network data gets copied
 * @param size is the byte capacity of buf
 * @param flags can have `MSG_OOB`, `MSG_PEEK`, `MSG_DONTWAIT`, `MSG_WAITALL`
 * @return number of bytes received, 0 on remote close, or -1 w/ errno
 * @raise EINTR if signal handler was called instead
 * @raise EINVAL if unknown bits were passed in `flags`
 * @raise EINVAL if flag isn't supported by host operating system
 * @raise EINVAL if `MSG_WAITALL` and `MSG_PEEK` were both passed
 * @raise EBADF if `fd` is an invalid file descriptor
 * @raise EAGAIN if `MSG_DONTWAIT` was passed and no data was available
 * @raise EAGAIN if `O_NONBLOCK` is in play and no data was available
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO on Linux or Windows)
 */
ssize_t recv(int fd, void *buf, size_t size, int flags) {
  ssize_t rc;
  BEGIN_CANCELATION_POINT;

  if ((flags & (MSG_WAITALL | MSG_PEEK)) == (MSG_WAITALL | MSG_PEEK)) {
    // this is possible on some OSes like Linux but it breaks FreeBSD
    // and Windows will raise EOPNOTSUPP when it gets passed together
    return einval();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotsock();
  } else if (!IsWindows()) {
    rc = sys_recvfrom(fd, buf, size, flags, 0, 0);
  } else if (__isfdopen(fd)) {
    if (__isfdkind(fd, kFdSocket)) {
      rc = sys_recv_nt(fd, (struct iovec[]){{buf, size}}, 1, flags);
    } else if (__isfdkind(fd, kFdFile)) {
      if (flags) {
        rc = einval();
      } else {
        rc = sys_read_nt(fd, (struct iovec[]){{buf, size}}, 1, -1);
      }
    } else {
      rc = enotsock();
    }
  } else {
    rc = ebadf();
  }

  END_CANCELATION_POINT;
  DATATRACE("recv(%d, [%#.*hhs%s], %'zu, %s) → %'ld% lm", fd,
            MAX(0, MIN(40, rc)), buf, rc > 40 ? "..." : "", size,
            DescribeMsg(flags), rc);
  return rc;
}
