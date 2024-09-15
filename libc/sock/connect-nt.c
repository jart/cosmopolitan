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
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/macros.h"
#include "libc/nt/errors.h"
#include "libc/nt/struct/fdset.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/struct/timeval.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/so.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

#define UNCONNECTED 0
#define CONNECTING  1
#define CONNECTED   2

__msabi extern typeof(__sys_getsockopt_nt) *const __imp_getsockopt;
__msabi extern typeof(__sys_ioctlsocket_nt) *const __imp_ioctlsocket;
__msabi extern typeof(__sys_select_nt) *const __imp_select;

textwindows static int sys_connect_nt_impl(struct Fd *f, const void *addr,
                                           uint32_t addrsize,
                                           sigset_t waitmask) {

  // check if already connected
  if (f->connecting == 2)
    return eisconn();

  // winsock requires bind() be called beforehand
  if (!f->isbound) {
    struct sockaddr_storage ss = {0};
    ss.ss_family = ((struct sockaddr *)addr)->sa_family;
    if (sys_bind_nt(f, &ss, sizeof(ss)) == -1)
      return -1;
  }

  if (f->connecting == UNCONNECTED) {

    // make sure winsock is in non-blocking mode
    uint32_t mode = 1;
    if (__imp_ioctlsocket(f->handle, FIONBIO, &mode))
      return __winsockerr();

    // perform non-blocking connect
    if (!WSAConnect(f->handle, addr, addrsize, 0, 0, 0, 0)) {
      f->connecting = CONNECTED;
      return 0;
    }

    // check for errors
    switch (WSAGetLastError()) {
      case WSAEISCONN:
        f->connecting = CONNECTED;
        return eisconn();
      case WSAEALREADY:
        f->connecting = CONNECTING;
        break;
      case WSAEWOULDBLOCK:
        break;
      default:
        return __winsockerr();
    }

    // handle non-blocking
    if (f->flags & O_NONBLOCK) {
      if (f->connecting == UNCONNECTED) {
        f->connecting = CONNECTING;
        return einprogress();
      } else {
        return ealready();
      }
    } else {
      f->connecting = CONNECTING;
    }
  }

  for (;;) {

    // check for signals and thread cancelation
    // connect() will restart if SA_RESTART is used
    if (!(f->flags & O_NONBLOCK))
      if (__sigcheck(waitmask, true) == -1)
        return -1;

    //
    // "Use select to determine the completion of the connection request
    //  by checking if the socket is writable."
    //
    //                  —Quoth MSDN § WSAConnect function
    //
    // "If a socket is processing a connect call (nonblocking), failure
    //  of the connect attempt is indicated in exceptfds (application
    //  must then call getsockopt SO_ERROR to determine the error value
    //  to describe why the failure occurred). This document does not
    //  define which other errors will be included."
    //
    //                  —Quoth MSDN § select function
    //
    struct NtFdSet wrfds;
    struct NtFdSet exfds;
    struct NtTimeval timeout;
    wrfds.fd_count = 1;
    wrfds.fd_array[0] = f->handle;
    exfds.fd_count = 1;
    exfds.fd_array[0] = f->handle;
    if (f->flags & O_NONBLOCK) {
      timeout.tv_sec = 0;
      timeout.tv_usec = 0;
    } else {
      timeout.tv_sec = POLL_INTERVAL_MS / 1000;
      timeout.tv_usec = POLL_INTERVAL_MS % 1000 * 1000;
    }
    int ready = __imp_select(1, 0, &wrfds, &exfds, &timeout);
    if (ready == -1)
      return __winsockerr();

    // check if we still need more time
    if (!ready) {
      if (f->flags & O_NONBLOCK) {
        return etimedout();
      } else {
        continue;
      }
    }

    // check if connect failed
    if (exfds.fd_count) {
      int err;
      uint32_t len = sizeof(err);
      if (__imp_getsockopt(f->handle, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
        return __winsockerr();
      if (!err)
        return eio();  // should be impossible
      errno = __dos2errno(err);
      return -1;
    }

    // handle successful connection
    if (!wrfds.fd_count)
      return eio();  // should be impossible
    f->connecting = CONNECTED;
    return 0;
  }
}

textwindows int sys_connect_nt(struct Fd *f, const void *addr,
                               uint32_t addrsize) {
  int rc;
  BLOCK_SIGNALS;
  rc = sys_connect_nt_impl(f, addr, addrsize, _SigMask);
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
