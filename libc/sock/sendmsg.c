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
 * Sends a message on a socket.
 *
 * @param fd is the file descriptor returned by socket()
 * @param msg is a pointer to a struct msghdr containing all the required
 *            parameters (the destination address, buffers, ...)
 * @param flags MSG_OOB, MSG_DONTROUTE, MSG_PARTIAL, MSG_NOSIGNAL, etc.
 * @return number of bytes transmitted, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @asyncsignalsafe
 */
ssize_t sendmsg(int fd, const struct msghdr *msg, int flags) {
  if (!IsWindows()) {
    if (IsBsd() && msg->msg_name) {
      /* An optional address is provided, convert it to the BSD form */
      char addr2[128];
      struct msghdr msg2;
      if (msg->msg_namelen > sizeof(addr2)) return einval();
      memcpy(&addr2[0], msg->msg_name, msg->msg_namelen);
      sockaddr2bsd(&addr2[0]);

      /* Copy all of msg (except for msg_name) into the new ephemeral local */
      memcpy(&msg2, msg, sizeof(msg2));
      msg2.msg_name = &addr2[0];
      return sys_sendmsg(fd, &msg2, flags);
    }
    /* else do the syscall */
    return sys_sendmsg(fd, msg, flags);
  } else {
    if (__isfdopen(fd)) {
      if (msg->msg_control) return einval(); /* control msg not supported */
      if (__isfdkind(fd, kFdSocket)) {
        return sys_sendto_nt(&g_fds.p[fd], msg->msg_iov, msg->msg_iovlen, flags,
                             msg->msg_name, msg->msg_namelen);
      } else if (__isfdkind(fd, kFdFile)) {
        return sys_write_nt(&g_fds.p[fd], msg->msg_iov, msg->msg_iovlen, -1);
      } else {
        return enotsock();
      }
    } else {
      return ebadf();
    }
  }
}
