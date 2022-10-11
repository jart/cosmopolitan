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
 * Returns details about network interface kernel granted socket.
 * @return 0 on success or -1 w/ errno
 * @see getpeername()
 */
int getsockname(int fd, struct sockaddr *out_addr, uint32_t *out_addrsize) {
  int rc;
  if (!out_addrsize || !out_addrsize ||
      (IsAsan() && (!__asan_is_valid(out_addrsize, 4) ||
                    !__asan_is_valid(out_addr, *out_addrsize)))) {
    rc = efault();
  } else if (!IsWindows()) {
    rc = sys_getsockname(fd, out_addr, out_addrsize);
  } else if (__isfdkind(fd, kFdSocket)) {
    rc = sys_getsockname_nt(&g_fds.p[fd], out_addr, out_addrsize);
  } else {
    rc = ebadf();
  }
  STRACE("getsockname(%d, [%s]) -> %d% lm", fd,
         DescribeSockaddr(out_addr, out_addrsize ? *out_addrsize : 0), rc);
  return rc;
}
