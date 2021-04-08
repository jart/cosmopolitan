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
 */
ssize_t recvmsg(int fd, struct msghdr *msg, int flags) {
  ssize_t got;
  if (!IsWindows()) {
    got = sys_recvmsg(fd, msg, flags);
    /* An address was provided, convert from BSD form */
    if (msg->msg_name && IsBsd() && got != -1) {
      sockaddr2linux(msg->msg_name);
    }
    return got;
  } else {
    if (__isfdopen(fd)) {
      if (msg->msg_control) return einval(); /* control msg not supported */
      if (__isfdkind(fd, kFdSocket)) {
        return sys_recvfrom_nt(&g_fds.p[fd], msg->msg_iov, msg->msg_iovlen,
                               flags, msg->msg_name, &msg->msg_namelen);
      } else if (__isfdkind(fd, kFdFile) && !msg->msg_name) { /* socketpair */
        if (flags) return einval();
        if ((got = sys_read_nt(&g_fds.p[fd], msg->msg_iov, msg->msg_iovlen,
                               -1)) != -1) {
          msg->msg_flags = 0;
          return got;
        } else {
          return -1;
        }
      } else {
        return enotsock();
      }
    } else {
      return ebadf();
    }
  }
}
