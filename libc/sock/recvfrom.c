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
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/errfuns.h"

/**
 * Receives data from network.
 *
 * This function blocks unless MSG_DONTWAIT is passed. In that case, the
 * non-error EWOULDBLOCK might be returned. It basically means we didn't
 * wait around to learn an amount of bytes were written that we know in
 * advance are guaranteed to be atomic.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is where received network data gets copied
 * @param size is the byte capacity of buf
 * @param flags can have `MSG_OOB`, `MSG_PEEK`, and `MSG_DONTWAIT`
 * @param opt_out_srcaddr receives the binary ip:port of the data's origin
 * @param opt_inout_srcaddrsize is srcaddr capacity which gets updated
 * @return number of bytes received, 0 on remote close, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @cancelationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO)
 */
ssize_t recvfrom(int fd, void *buf, size_t size, int flags,
                 struct sockaddr *opt_out_srcaddr,
                 uint32_t *opt_inout_srcaddrsize) {
  ssize_t rc;
  struct sockaddr_storage addr = {0};
  uint32_t addrsize = sizeof(addr);
  BEGIN_CANCELATION_POINT;

  if (IsAsan() && !__asan_is_valid(buf, size)) {
    rc = efault();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotsock();
  } else if (!IsWindows()) {
    rc = sys_recvfrom(fd, buf, size, flags, &addr, &addrsize);
  } else if (__isfdopen(fd)) {
    if (__isfdkind(fd, kFdSocket)) {
      rc = sys_recvfrom_nt(fd, (struct iovec[]){{buf, size}}, 1, flags, &addr,
                           &addrsize);
    } else if (__isfdkind(fd, kFdFile) && !opt_out_srcaddr) { /* socketpair */
      if (!flags) {
        rc = sys_read_nt(fd, (struct iovec[]){{buf, size}}, 1, -1);
      } else {
        rc = einval();
      }
    } else {
      rc = enotsock();
    }
  } else {
    rc = ebadf();
  }

  if (rc != -1) {
    if (IsBsd()) {
      __convert_bsd_to_sockaddr(&addr);
    }
    __write_sockaddr(&addr, opt_out_srcaddr, opt_inout_srcaddrsize);
  }

  END_CANCELATION_POINT;
  DATATRACE("recvfrom(%d, [%#.*hhs%s], %'zu, %#x) → %'ld% lm", fd,
            MAX(0, MIN(40, rc)), buf, rc > 40 ? "..." : "", size, flags, rc);
  return rc;
}
