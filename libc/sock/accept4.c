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
 * Creates client socket file descriptor for incoming connection.
 *
 * @param fd is the server socket file descriptor
 * @param out_addr will receive the remote address
 * @param inout_addrsize provides and receives out_addr's byte length
 * @param flags can have SOCK_{CLOEXEC,NONBLOCK}, which may apply to
 *     both the newly created socket and the server one
 * @return client fd which needs close(), or -1 w/ errno
 * @cancellationpoint
 * @asyncsignalsafe
 * @restartable (unless SO_RCVTIMEO)
 */
int accept4(int fd, struct sockaddr *out_addr, uint32_t *inout_addrsize,
            int flags) {
  int rc;
  char addrbuf[72];
  BEGIN_CANCELLATION_POINT;

  if (!out_addr || !inout_addrsize ||
      (IsAsan() && !__asan_is_valid(out_addr, *inout_addrsize))) {
    rc = efault();
  } else if (!IsWindows()) {
    rc = sys_accept4(fd, out_addr, inout_addrsize, flags);
  } else if (__isfdkind(fd, kFdSocket)) {
    rc = sys_accept_nt(&g_fds.p[fd], out_addr, inout_addrsize, flags);
  } else {
    rc = ebadf();
  }

  END_CANCELLATION_POINT;
  STRACE("accept4(%d, [%s]) -> %d% lm", fd,
         DescribeSockaddr(out_addr, inout_addrsize ? *inout_addrsize : 0), rc);
  return rc;
}
