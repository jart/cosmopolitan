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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/sigset.internal.h"
#include "libc/cosmo.h"
#include "libc/errno.h"
#include "libc/nt/enum/wsaid.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/struct/sockaddr.h"
#include "libc/sock/wsaid.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/thread/thread.h"
#ifdef __x86_64__

__msabi extern typeof(__sys_setsockopt_nt) *const __imp_setsockopt;
__msabi extern typeof(__sys_closesocket_nt) *const __imp_closesocket;

union AcceptExAddr {
  struct sockaddr_storage addr;
  char buf[sizeof(struct sockaddr_storage) + 16];
};

struct AcceptExBuffer {
  union AcceptExAddr local;
  union AcceptExAddr remote;
};

struct AcceptResources {
  int64_t handle;
};

struct AcceptArgs {
  int64_t listensock;
  struct AcceptExBuffer *buffer;
};

static struct {
  atomic_uint once;
  bool32 (*__msabi lpAcceptEx)(
      int64_t sListenSocket, int64_t sAcceptSocket,
      void *out_lpOutputBuffer /*[recvlen+local+remoteaddrlen]*/,
      uint32_t dwReceiveDataLength, uint32_t dwLocalAddressLength,
      uint32_t dwRemoteAddressLength, uint32_t *out_lpdwBytesReceived,
      struct NtOverlapped *inout_lpOverlapped);
} g_acceptex;

static void acceptex_init(void) {
  static struct NtGuid AcceptExGuid = WSAID_ACCEPTEX;
  g_acceptex.lpAcceptEx = __get_wsaid(&AcceptExGuid);
}

static void sys_accept_nt_unwind(void *arg) {
  struct AcceptResources *resources = arg;
  if (resources->handle != -1) {
    __imp_closesocket(resources->handle);
  }
}

static int sys_accept_nt_start(int64_t handle, struct NtOverlapped *overlap,
                               uint32_t *flags, void *arg) {
  struct AcceptArgs *args = arg;
  cosmo_once(&g_acceptex.once, acceptex_init);
  if (g_acceptex.lpAcceptEx(args->listensock, handle, args->buffer, 0,
                            sizeof(args->buffer->local),
                            sizeof(args->buffer->remote), 0, overlap)) {
    return 0;
  } else {
    return -1;
  }
}

textwindows int sys_accept_nt(struct Fd *f, struct sockaddr_storage *addr,
                              int accept4_flags) {
  int client = -1;
  sigset_t m = __sig_block();
  struct AcceptResources resources = {-1};
  pthread_cleanup_push(sys_accept_nt_unwind, &resources);

  // creates resources for child socket
  // inherit the listener configuration
  if ((resources.handle = WSASocket(f->family, f->type, f->protocol, 0, 0,
                                    kNtWsaFlagOverlapped)) == -1) {
    client = __winsockerr();
    goto Finish;
  }

  // accept network connection
  // this operation can re-enter, interrupt, cancel, block, timeout, etc.
  struct AcceptExBuffer buffer;
  ssize_t bytes_received = __winsock_block(
      resources.handle, 0, !!(f->flags & O_NONBLOCK), f->rcvtimeo, m,
      sys_accept_nt_start, &(struct AcceptArgs){f->handle, &buffer});
  if (bytes_received == -1) {
    __imp_closesocket(resources.handle);
    goto Finish;
  }

  // inherit properties of listening socket
  // errors ignored as if f->handle was created before forking
  // this fails with WSAENOTSOCK
  __imp_setsockopt(resources.handle, SOL_SOCKET, kNtSoUpdateAcceptContext,
                   &f->handle, sizeof(f->handle));

  // create file descriptor for new socket
  // don't inherit the file open mode bits
  int oflags = 0;
  if (accept4_flags & SOCK_CLOEXEC)
    oflags |= O_CLOEXEC;
  if (accept4_flags & SOCK_NONBLOCK)
    oflags |= O_NONBLOCK;
  client = __reservefd(-1);
  g_fds.p[client].flags = oflags;
  g_fds.p[client].mode = 0140666;
  g_fds.p[client].family = f->family;
  g_fds.p[client].type = f->type;
  g_fds.p[client].protocol = f->protocol;
  g_fds.p[client].sndtimeo = f->sndtimeo;
  g_fds.p[client].rcvtimeo = f->rcvtimeo;
  g_fds.p[client].handle = resources.handle;
  resources.handle = -1;
  memcpy(addr, &buffer.remote.addr, sizeof(*addr));
  g_fds.p[client].kind = kFdSocket;

Finish:
  pthread_cleanup_pop(false);
  __sig_unblock(m);
  if (client == -1 && errno == ECONNRESET) {
    errno = ECONNABORTED;
  }
  return client;
}

#endif /* __x86_64__ */
