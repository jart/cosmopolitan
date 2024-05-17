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
#include "libc/atomic.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/nt/enum/wsaid.h"
#include "libc/nt/errors.h"
#include "libc/nt/struct/guid.h"
#include "libc/nt/struct/overlapped.h"
#include "libc/nt/thread.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sock/wsaid.internal.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"

#ifdef __x86_64__
#include "libc/sock/yoink.inc"

__msabi extern typeof(__sys_setsockopt_nt) *const __imp_setsockopt;

struct ConnectArgs {
  const void *addr;
  uint32_t addrsize;
};

static struct {
  atomic_uint once;
  bool32 (*__msabi lpConnectEx)(int64_t hSocket, const struct sockaddr *name,
                                int namelen, const void *opt_lpSendBuffer,
                                uint32_t dwSendDataLength,
                                uint32_t *opt_out_lpdwBytesSent,
                                struct NtOverlapped *lpOverlapped);
} g_connectex;

static void connectex_init(void) {
  static struct NtGuid ConnectExGuid = WSAID_CONNECTEX;
  g_connectex.lpConnectEx = __get_wsaid(&ConnectExGuid);
}

void sys_connect_nt_cleanup(struct Fd *f, bool cancel) {
  struct NtOverlapped *overlap;
  if ((overlap = f->connect_op)) {
    uint32_t got, flags;
    if (cancel)
      CancelIoEx(f->handle, overlap);
    if (WSAGetOverlappedResult(f->handle, overlap, &got, cancel, &flags) ||
        WSAGetLastError() != kNtErrorIoIncomplete) {
      WSACloseEvent(overlap->hEvent);
      free(overlap);
      f->connect_op = 0;
    }
  }
}

static int sys_connect_nt_start(int64_t hSocket,
                                struct NtOverlapped *lpOverlapped,
                                uint32_t *flags, void *arg) {
  struct ConnectArgs *args = arg;
  if (g_connectex.lpConnectEx(hSocket, args->addr, args->addrsize, 0, 0, 0,
                              lpOverlapped)) {
    return 0;
  } else {
    return -1;
  }
}

static textwindows int sys_connect_nt_impl(struct Fd *f, const void *addr,
                                           uint32_t addrsize, sigset_t mask) {

  // get connect function from winsock api
  cosmo_once(&g_connectex.once, connectex_init);

  // fail if previous connect() is still in progress
  if (f->connect_op)
    return ealready();

  // ConnectEx() requires bind() be called beforehand
  if (!f->isbound) {
    struct sockaddr_storage ss = {0};
    ss.ss_family = ((struct sockaddr *)addr)->sa_family;
    if (sys_bind_nt(f, &ss, sizeof(ss)) == -1)
      return -1;
  }

  // perform normal connect
  if (!(f->flags & O_NONBLOCK)) {
    ssize_t rc = __winsock_block(f->handle, 0, false, f->sndtimeo, mask,
                                 sys_connect_nt_start,
                                 &(struct ConnectArgs){addr, addrsize});
    if (rc == -1 && errno == EAGAIN) {
      // return ETIMEDOUT if SO_SNDTIMEO elapsed
      // note that Linux will return EINPROGRESS
      errno = etimedout();
    } else if (!rc) {
      __imp_setsockopt(f->handle, SOL_SOCKET, kNtSoUpdateConnectContext, 0, 0);
    }
    return rc;
  }

  // perform nonblocking connect(), i.e.
  // 1. connect(O_NONBLOCK) → EINPROGRESS
  // 2. poll(POLLOUT)
  bool32 ok;
  struct NtOverlapped *overlap = calloc(1, sizeof(struct NtOverlapped));
  if (!overlap)
    return -1;
  overlap->hEvent = WSACreateEvent();
  ok = g_connectex.lpConnectEx(f->handle, addr, addrsize, 0, 0, 0, overlap);
  if (ok) {
    uint32_t dwBytes, dwFlags;
    ok = WSAGetOverlappedResult(f->handle, overlap, &dwBytes, false, &dwFlags);
    WSACloseEvent(overlap->hEvent);
    free(overlap);
    if (!ok) {
      return __winsockerr();
    }
    __imp_setsockopt(f->handle, SOL_SOCKET, kNtSoUpdateConnectContext, 0, 0);
    return 0;
  } else if (WSAGetLastError() == kNtErrorIoPending) {
    f->connect_op = overlap;
    return einprogress();
  } else {
    WSACloseEvent(overlap->hEvent);
    free(overlap);
    return __winsockerr();
  }
}

textwindows int sys_connect_nt(struct Fd *f, const void *addr,
                               uint32_t addrsize) {
  sigset_t mask = __sig_block();
  int rc = sys_connect_nt_impl(f, addr, addrsize, mask);
  __sig_unblock(mask);
  return rc;
}

#endif /* __x86_64__ */
