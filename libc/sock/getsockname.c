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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/errors.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/sock.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/struct/sockaddr.internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/errfuns.h"

__msabi extern typeof(__sys_getsockname_nt) *const __imp_getsockname;
__msabi extern typeof(__sys_getpeername_nt) *const __imp_getpeername;

static int __getsockpeername(int fd, struct sockaddr *out_addr,
                             uint32_t *inout_addrsize, const char *name,
                             int impl_sysv(int, void *, uint32_t *),
                             int (*__msabi impl_win32)(uint64_t, void *,
                                                       uint32_t *)) {
  int rc;
  struct sockaddr_storage ss = {0};
  uint32_t size = sizeof(ss);

  if (IsWindows()) {
    if (__isfdkind(fd, kFdSocket)) {
      if ((rc = impl_win32(g_fds.p[fd].handle, &ss, &size))) {
        if (impl_win32 == __imp_getsockname && WSAGetLastError() == WSAEINVAL) {
          // The socket has not been bound to an address with bind, or
          // ADDR_ANY is specified in bind but connection has not yet
          // occurred. -MSDN
          ss.ss_family = g_fds.p[fd].family;
          rc = 0;
        } else {
          rc = __winsockerr();
        }
      }
    } else {
      rc = ebadf();
    }
  } else {
    rc = impl_sysv(fd, &ss, &size);
  }

  if (!rc) {
    if (IsBsd()) {
      __convert_bsd_to_sockaddr(&ss);
    }
    __write_sockaddr(&ss, out_addr, inout_addrsize);
  }

  STRACE("%s(%d, [%s]) -> %d% lm", name, fd,
         DescribeSockaddr(out_addr, inout_addrsize ? *inout_addrsize : 0), rc);
  return rc;
}

/**
 * Returns details about network interface kernel granted socket.
 * @return 0 on success or -1 w/ errno
 * @see getpeername()
 */
int getsockname(int fd, struct sockaddr *out_addr, uint32_t *inout_addrsize) {
  return __getsockpeername(fd, out_addr, inout_addrsize, "getsockname",
                           __sys_getsockname, __imp_getsockname);
}

/**
 * Returns details about remote end of connected socket.
 * @return 0 on success or -1 w/ errno
 * @see getsockname()
 */
int getpeername(int fd, struct sockaddr *out_addr, uint32_t *inout_addrsize) {
  return __getsockpeername(fd, out_addr, inout_addrsize, "getpeername",
                           __sys_getpeername, __imp_getpeername);
}
