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
 * This function blocks unless MSG_DONTWAIT is passed.
 *
 * @param fd is the file descriptor returned by socket()
 * @param buf is where received network data gets copied
 * @param size is the byte capacity of buf
 * @param flags can have MSG_{WAITALL,DONTROUTE,PEEK,OOB}, etc.
 * @param opt_out_srcaddr receives the binary ip:port of the data's origin
 * @param opt_inout_srcaddrsize is srcaddr capacity which gets updated
 * @return number of bytes received, 0 on remote close, or -1 w/ errno
 * @error EINTR, EHOSTUNREACH, ECONNRESET (UDP ICMP Port Unreachable),
 *     EPIPE (if MSG_NOSIGNAL), EMSGSIZE, ENOTSOCK, EFAULT, etc.
 * @cancellationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO)
 */
ssize_t recvfrom(int fd, void *buf, size_t size, int flags,
                 struct sockaddr *opt_out_srcaddr,
                 uint32_t *opt_inout_srcaddrsize) {
  ssize_t rc;
  uint32_t sz;
  union sockaddr_storage_bsd bsd;
  BEGIN_CANCELLATION_POINT;

  if (IsAsan() &&
      (!__asan_is_valid(buf, size) ||
       (opt_out_srcaddr &&
        (!__asan_is_valid(opt_inout_srcaddrsize,
                          sizeof(*opt_inout_srcaddrsize)) ||
         !__asan_is_valid(opt_out_srcaddr, *opt_inout_srcaddrsize))))) {
    rc = efault();
  } else if (!IsWindows()) {
    if (!IsBsd() || !opt_out_srcaddr) {
      rc = sys_recvfrom(fd, buf, size, flags, opt_out_srcaddr,
                        opt_inout_srcaddrsize);
    } else {
      sz = sizeof(bsd);
      if ((rc = sys_recvfrom(fd, buf, size, flags, &bsd, &sz)) != -1) {
        sockaddr2linux(&bsd, sz, (void *)opt_out_srcaddr,
                       opt_inout_srcaddrsize);
      }
    }
  } else if (__isfdopen(fd)) {
    if (__isfdkind(fd, kFdSocket)) {
      rc = sys_recvfrom_nt(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1,
                           flags, opt_out_srcaddr, opt_inout_srcaddrsize);
    } else if (__isfdkind(fd, kFdFile) && !opt_out_srcaddr) { /* socketpair */
      if (flags) {
        rc = einval();
      } else {
        rc = sys_read_nt(&g_fds.p[fd], (struct iovec[]){{buf, size}}, 1, -1);
      }
    } else {
      rc = enotsock();
    }
  } else {
    rc = ebadf();
  }

  END_CANCELLATION_POINT;
  DATATRACE("recvfrom(%d, [%#.*hhs%s], %'zu, %#x) → %'ld% lm", fd,
            MAX(0, MIN(40, rc)), buf, rc > 40 ? "..." : "", size, flags, rc);
  return rc;
}
