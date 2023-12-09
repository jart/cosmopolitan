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
#include "libc/assert.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/iovec.h"
#include "libc/calls/struct/iovec.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/strace.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/msghdr.h"
#include "libc/sock/struct/msghdr.internal.h"
#include "libc/sock/struct/sockaddr.internal.h"
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
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO)
 */
ssize_t sendmsg(int fd, const struct msghdr *msg, int flags) {
  int64_t rc;
  struct msghdr msg2;
  union sockaddr_storage_bsd bsd;

  BEGIN_CANCELATION_POINT;
  if (IsAsan() && !__asan_is_valid_msghdr(msg)) {
    rc = efault();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotsock();
  } else if (!IsWindows()) {
    if (IsBsd() && msg->msg_name) {
      memcpy(&msg2, msg, sizeof(msg2));
      if (!(rc = sockaddr2bsd(msg->msg_name, msg->msg_namelen, &bsd,
                              &msg2.msg_namelen))) {
        msg2.msg_name = &bsd.sa;
        rc = sys_sendmsg(fd, &msg2, flags);
      }
    } else {
      rc = sys_sendmsg(fd, msg, flags);
    }
  } else if (__isfdopen(fd)) {
    if (msg->msg_control) {
      rc = einval(); /* control msg not supported */
    } else if (__isfdkind(fd, kFdSocket)) {
      rc = sys_sendto_nt(fd, msg->msg_iov, msg->msg_iovlen, flags,
                         msg->msg_name, msg->msg_namelen);
    } else if (__isfdkind(fd, kFdFile)) {  // e.g. socketpair
      rc = sys_write_nt(fd, msg->msg_iov, msg->msg_iovlen, -1);
    } else {
      rc = enotsock();
    }
  } else {
    rc = ebadf();
  }
  END_CANCELATION_POINT;

#if SYSDEBUG && _DATATRACE
  // TODO(jart): Write a DescribeMsg() function.
  if (strace_enabled(0) > 0) {
    kprintf(STRACE_PROLOGUE "sendmsg(%d, ", fd);
    if ((!IsAsan() && kisdangerous(msg)) ||
        (IsAsan() && !__asan_is_valid(msg, sizeof(*msg)))) {
      kprintf("%p", msg);
    } else {
      kprintf("{");
      kprintf(".name=%#.*hhs, ", msg->msg_namelen, msg->msg_name);
      if (msg->msg_controllen)
        kprintf(", .control=%#.*hhs, ", msg->msg_controllen, msg->msg_control);
      if (msg->msg_flags) kprintf(".flags=%#x, ", msg->msg_flags);
      kprintf(", .iov=%s",
              DescribeIovec(rc != -1 ? rc : -2, msg->msg_iov, msg->msg_iovlen));
    }
    kprintf(", %#x) → %'ld% m\n", flags, rc);
  }
#endif

  return rc;
}
