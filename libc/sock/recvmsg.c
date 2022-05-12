/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/dce.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

/**
 * Sends a message from a socket.
 *
 * @param fd is the file descriptor returned by socket()
 * @param msg is a pointer to a struct msghdr containing all the allocated
 *            buffers where to store incoming data.
 * @param flags MSG_OOB, MSG_DONTROUTE, MSG_PARTIAL, MSG_NOSIGNAL, etc.
 * @return number of bytes received, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO)
 */
ssize_t recvmsg(int fd, struct msghdr *msg, int flags) {
  ssize_t rc, got;
  if (IsAsan() && !__asan_is_valid_msghdr(msg)) {
    rc = efault();
  } else if (!IsWindows()) {
    got = sys_recvmsg(fd, msg, flags);
    // An address was provided, convert from BSD form
    if (msg->msg_name && IsBsd() && got != -1) {
      sockaddr2linux(msg->msg_name);
    }
    rc = got;
  } else if (__isfdopen(fd)) {
    if (!msg->msg_control) {
      if (__isfdkind(fd, kFdSocket)) {
        rc = sys_recvfrom_nt(&g_fds.p[fd], msg->msg_iov, msg->msg_iovlen, flags,
                             msg->msg_name, &msg->msg_namelen);
      } else if (__isfdkind(fd, kFdFile) && !msg->msg_name) { /* socketpair */
        if (!flags) {
          if ((got = sys_read_nt(&g_fds.p[fd], msg->msg_iov, msg->msg_iovlen,
                                 -1)) != -1) {
            msg->msg_flags = 0;
            rc = got;
          } else {
            rc = -1;
          }
        } else {
          rc = einval();  // flags not supported on nt
        }
      } else {
        rc = enotsock();
      }
    } else {
      rc = einval();  // control msg not supported on nt
    }
  } else {
    rc = ebadf();
  }
#if defined(SYSDEBUG) && _DATATRACE
  if (__strace > 0) {
    if (!msg || (rc == -1 && errno == EFAULT)) {
      DATATRACE("recvmsg(%d, %p, %#x) → %'ld% m", fd, msg, flags, rc);
    } else {
      kprintf(STRACE_PROLOGUE "recvmsg(%d, [{");
      if (msg->msg_namelen)
        kprintf(".name=%#.*hhs, ", msg->msg_namelen, msg->msg_name);
      if (msg->msg_controllen)
        kprintf(".control=%#.*hhs, ", msg->msg_controllen, msg->msg_control);
      if (msg->msg_flags) kprintf(".flags=%#x, ", msg->msg_flags);
      kprintf(".iov=", fd);
      DescribeIov(msg->msg_iov, msg->msg_iovlen, rc != -1 ? rc : 0);
      kprintf("}], %#x) → %'ld% m\n", flags, rc);
    }
  }
#endif
  return rc;
}
