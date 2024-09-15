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
#include "libc/calls/struct/sigset.internal.h"
#include "libc/calls/syscall_support-nt.internal.h"
#include "libc/errno.h"
#include "libc/nt/errors.h"
#include "libc/nt/struct/pollfd.h"
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/winsock.h"
#include "libc/sock/internal.h"
#include "libc/sock/syscall_fd.internal.h"
#include "libc/sysv/consts/fio.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/poll.h"
#include "libc/sysv/consts/sock.h"
#include "libc/sysv/consts/sol.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

__msabi extern typeof(__sys_ioctlsocket_nt) *const __imp_ioctlsocket;

textwindows static int sys_accept_nt_impl(struct Fd *f,
                                          struct sockaddr_storage *addr,
                                          int accept4_flags,
                                          sigset_t waitmask) {
  int64_t handle;
  int client = -1;

  // accepting sockets must always be non-blocking at the os level. this
  // is because WSAAccept doesn't support overlapped i/o operations. the
  // AcceptEx function claims to support overlapped i/o however it can't
  // be canceled by CancelIoEx, which makes it quite useless to us sadly
  // this can't be called in listen(), because then fork() will break it
  uint32_t mode = 1;
  if (__imp_ioctlsocket(f->handle, FIONBIO, &mode))
    return __winsockerr();

  for (;;) {

    // perform non-blocking accept
    int32_t addrsize = sizeof(*addr);
    struct sockaddr *paddr = (struct sockaddr *)addr;
    if ((handle = WSAAccept(f->handle, paddr, &addrsize, 0, 0)) != -1)
      break;

    // return on genuine errors
    uint32_t err = WSAGetLastError();
    if (err != WSAEWOULDBLOCK) {
      errno = __dos2errno(err);
      if (errno == ECONNRESET)
        errno = ECONNABORTED;
      return -1;
    }

    // check for non-blocking
    if (f->flags & O_NONBLOCK)
      return eagain();

    // check for signals and thread cancelation
    // accept() will restart if SA_RESTART is used
    if (__sigcheck(waitmask, true) == -1)
      return -1;

    // time to block
    struct sys_pollfd_nt fds[1] = {{f->handle, POLLIN}};
    if (WSAPoll(fds, 1, POLL_INTERVAL_MS) == -1)
      return __winsockerr();
  }

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
  g_fds.p[client].handle = handle;
  g_fds.p[client].kind = kFdSocket;
  return client;
}

textwindows int sys_accept_nt(struct Fd *f, struct sockaddr_storage *addr,
                              int accept4_flags) {
  int rc;
  BLOCK_SIGNALS;
  rc = sys_accept_nt_impl(f, addr, accept4_flags, _SigMask);
  ALLOW_SIGNALS;
  return rc;
}

#endif /* __x86_64__ */
